//@category Decompile

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.feature.fid.db.FidQueryService;
import ghidra.feature.fid.plugin.HashLookupListMode;
import ghidra.feature.fid.service.FidMatch;
import ghidra.feature.fid.service.FidProgramSeeker;
import ghidra.feature.fid.service.FidSearchResult;
import ghidra.feature.fid.service.FidService;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.data.Array;
import ghidra.program.model.data.DataType;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.listing.Program;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolType;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public class DecompileToSingleC extends GhidraScript {
    private static final float FID_SCORE_THRESHOLD = 0.1f;
    private FidService fidService = null;
    private FidQueryService fidQueryService = null;
    private FidProgramSeeker fidProgramSeeker = null;

    private String formatAddress(Address address) {
        long offset = address.getOffset();
        if (offset >= 0 && offset <= 0xffffffffL) {
            return String.format("0x%08X", offset);
        }
        return String.format("0x%016X", offset);
    }

    private String targetFromProgram(Program program) {
        String name = program.getName();
        int dot = name.lastIndexOf('.');
        if (dot > 0) {
            name = name.substring(0, dot);
        }
        name = name.replaceAll("[^A-Za-z0-9]+", "_");
        if (name.isEmpty()) {
            name = "UNKNOWN";
        }
        return name.toUpperCase();
    }

    private boolean isLibraryFunction(Function func) {
        if (func.isExternal() || func.isThunk()) {
            return true;
        }
        if (currentProgram.getSymbolTable().isExternalEntryPoint(func.getEntryPoint())) {
            return false;
        }

        Symbol sym = func.getSymbol();
        if (sym != null && sym.getSource() == SourceType.IMPORTED) {
            return true;
        }
        Namespace ns = func.getParentNamespace();
        if (ns != null && (ns.isExternal() || ns.isLibrary())) {
            return true;
        }
        return fidProgramSeeker != null && fidMatchIsHighConfidence(func);
    }

    private boolean fidMatchIsHighConfidence(Function func) {
        try {
            FidSearchResult result = fidProgramSeeker.searchFunction(func, monitor);
            if (result == null || result.matches == null || result.matches.isEmpty()) {
                return false;
            }
            float bestScore = -1.0f;
            for (FidMatch match : result.matches) {
                HashLookupListMode mode = match.getPrimaryFunctionMatchMode();
                if (mode != HashLookupListMode.FULL && mode != HashLookupListMode.SPECIFIC) {
                    continue;
                }
                float score = match.getOverallScore();
                if (score > bestScore) {
                    bestScore = score;
                }
            }
            return bestScore >= FID_SCORE_THRESHOLD;
        } catch (Exception e) {
            return false;
        }
    }

    private Map<Long, GlobalInfo> collectReferencedGlobals(Program program, List<Function> functions) {
        Map<Long, GlobalInfo> globals = new TreeMap<>();
        ReferenceManager rm = program.getReferenceManager();
        Listing listing = program.getListing();

        for (Function func : functions) {
            if (isLibraryFunction(func)) {
                continue;
            }
            AddressIterator addrIter = rm.getReferenceSourceIterator(func.getBody(), true);
            while (addrIter.hasNext()) {
                Address fromAddr = addrIter.next();
                if (listing.getInstructionAt(fromAddr) == null) {
                    continue;
                }
                Reference[] refs = rm.getReferencesFrom(fromAddr);
                for (Reference ref : refs) {
                    if (!(ref.getReferenceType().isData() ||
                        ref.getReferenceType().isRead() ||
                        ref.getReferenceType().isWrite())) {
                        continue;
                    }
                    Address toAddr = ref.getToAddress();
                    if (toAddr == null || !program.getMemory().contains(toAddr)) {
                        continue;
                    }
                    if (program.getListing().getFunctionAt(toAddr) != null) {
                        continue;
                    }
                    Symbol sym = program.getSymbolTable().getPrimarySymbol(toAddr);
                    if (sym == null || sym.getSymbolType() == SymbolType.FUNCTION || sym.isExternal()) {
                        continue;
                    }

                    Data data = listing.getDefinedDataAt(toAddr);
                    if (data == null) {
                        continue;
                    }

                    String name = sym.getName();
                    if (name.startsWith("LAB_") || name.startsWith("UNK_")) {
                        continue;
                    }

                    long key = toAddr.getOffset();
                    if (globals.containsKey(key)) {
                        continue;
                    }

                    DataType dataType = data.getDataType();
                    String typeName = null;
                    boolean isCharArray = false;
                    String stringValue = null;
                    if (dataType != null) {
                        String display = dataType.getDisplayName();
                        if (display != null && !display.isEmpty()) {
                            typeName = display;
                        }
                        if (dataType instanceof Array) {
                            DataType base = ((Array) dataType).getDataType();
                            if (base != null) {
                                String baseName = base.getDisplayName();
                                if (baseName != null && baseName.toLowerCase().contains("char")) {
                                    isCharArray = true;
                                }
                            }
                        }
                        if (display != null) {
                            String lower = display.toLowerCase();
                            if (lower.contains("string")) {
                                isCharArray = true;
                            }
                        }
                    }

                    try {
                        Object value = data.getValue();
                        if (value instanceof String) {
                            stringValue = (String) value;
                        }
                    } catch (Exception e) {
                        // ignore
                    }
                    byte[] bytes = null;
                    int length = data.getLength();
                    if (length > 0) {
                        try {
                            bytes = new byte[length];
                            program.getMemory().getBytes(toAddr, bytes);
                        } catch (Exception e) {
                            bytes = null;
                        }
                    }
                    globals.put(key, new GlobalInfo(toAddr, name, typeName, bytes, isCharArray, stringValue));
                }
            }
        }

        return globals;
    }

    private void writeGlobals(BufferedWriter writer, String target, Map<Long, GlobalInfo> globals) throws IOException {
        for (GlobalInfo info : globals.values()) {
            writer.write(String.format("// GLOBAL: %s %s", target, formatAddress(info.address)));
            writer.newLine();
            if (info.bytes != null && info.bytes.length > 0) {
                writeInitializedGlobal(writer, info);
            } else {
                String typeName = (info.typeName != null) ? info.typeName : "unsigned char";
                writer.write("extern " + typeName + " " + info.name + ";");
            }
            writer.newLine();
            writer.newLine();
        }
    }

    private static class GlobalInfo {
        final Address address;
        final String name;
        final String typeName;
        final byte[] bytes;
        final boolean isCharArray;
        final String stringValue;

        GlobalInfo(Address address, String name, String typeName, byte[] bytes, boolean isCharArray, String stringValue) {
            this.address = address;
            this.name = name;
            this.typeName = typeName;
            this.bytes = bytes;
            this.isCharArray = isCharArray;
            this.stringValue = stringValue;
        }
    }

    private void writeInitializedGlobal(BufferedWriter writer, GlobalInfo info) throws IOException {
        int length = info.bytes.length;
        String typeName = info.typeName;
        if (typeName != null &&
            (length == 1 || length == 2 || length == 4 || length == 8) &&
            !typeName.contains("struct") && !typeName.contains("union") &&
            !typeName.contains("[")) {
            long value = 0;
            for (int i = 0; i < length; i++) {
                value |= ((long) info.bytes[i] & 0xffL) << (8 * i);
            }
            String hex = String.format("0x%0" + (length * 2) + "X", value);
            writer.write(typeName + " " + info.name + " = " + hex + ";");
            return;
        }

        if (info.stringValue != null) {
            String name = normalizeStringSymbolName(info.name);
            writer.write("char " + name + "[] = ");
            writer.write("\"" + escapeCString(info.stringValue.getBytes()) + "\";");
            return;
        }

        if ((info.isCharArray || isCharArrayType(typeName)) && isPrintableCString(info.bytes)) {
            String name = normalizeStringSymbolName(info.name);
            writer.write("char " + name + "[] = ");
            writer.write("\"" + escapeCString(info.bytes) + "\";");
            return;
        }

        writer.write("unsigned char " + info.name + "[" + length + "] = {");
        writer.newLine();
        for (int i = 0; i < length; i++) {
            if (i % 12 == 0) {
                writer.write("  ");
            }
            writer.write(String.format("0x%02X", info.bytes[i]));
            if (i + 1 < length) {
                writer.write(", ");
            }
            if (i % 12 == 11 || i + 1 == length) {
                writer.newLine();
            }
        }
        writer.write("};");
    }

    private boolean isCharArrayType(String typeName) {
        String lower = typeName.toLowerCase();
        return lower.contains("char") && lower.contains("[");
    }

    private String normalizeStringSymbolName(String name) {
        if (name == null || !name.startsWith("s_")) {
            return name;
        }
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < name.length(); i++) {
            char c = name.charAt(i);
            if (Character.isLetterOrDigit(c) || c == '_' || c == '>') {
                sb.append(c);
            } else {
                sb.append('_');
            }
        }
        return sb.toString();
    }

    private boolean isPrintableCString(byte[] bytes) {
        int length = bytes.length;
        if (length == 0) {
            return false;
        }
        int end = -1;
        for (int i = 0; i < length; i++) {
            if (bytes[i] == 0) {
                end = i;
                break;
            }
        }
        if (end <= 0) {
            return false;
        }
        for (int i = 0; i < end; i++) {
            int b = bytes[i] & 0xff;
            if (b < 0x20 || b > 0x7e) {
                return false;
            }
        }
        return true;
    }

    private String escapeCString(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < bytes.length; i++) {
            int b = bytes[i] & 0xff;
            if (b == 0) {
                break;
            }
            switch (b) {
                case '\\':
                    sb.append("\\\\");
                    break;
                case '"':
                    sb.append("\\\"");
                    break;
                case '\n':
                    sb.append("\\n");
                    break;
                case '\r':
                    sb.append("\\r");
                    break;
                case '\t':
                    sb.append("\\t");
                    break;
                default:
                    sb.append((char) b);
                    break;
            }
        }
        return sb.toString();
    }

    private void writeLibraryEntry(BufferedWriter writer, String target, Function func) throws IOException {
        String addr = formatAddress(func.getEntryPoint());
        String name = func.getName();
        boolean isMsvcSymbol = name.startsWith("?");

        if (isMsvcSymbol) {
            writer.write(String.format("// LIBRARY: %s %s", target, addr));
        } else {
            writer.write(String.format("// LIBRARY: %s %s SYMBOL", target, addr));
        }
        writer.newLine();
        writer.write("// " + name);
        writer.newLine();
        writer.newLine();
    }

    private void writeDecompiledFunction(
        BufferedWriter writer,
        String target,
        Function func,
        DecompInterface decompiler
    ) throws IOException {
        String addr = formatAddress(func.getEntryPoint());
        writer.write(String.format("// FUNCTION: %s %s", target, addr));
        writer.newLine();

        DecompileResults res = decompiler.decompileFunction(func, 60, monitor);
        if (res != null && res.decompileCompleted() && res.getDecompiledFunction() != null) {
            String cCode = res.getDecompiledFunction().getC();
            writer.write(cCode.trim());
            writer.newLine();
            writer.newLine();
            return;
        }

        String proto;
        try {
            proto = func.getSignature().getPrototypeString();
        } catch (Exception e) {
            proto = func.getName() + "()";
        }

        writer.write(proto);
        writer.newLine();
        writer.write("{");
        writer.newLine();
        writer.write("}");
        writer.newLine();
        writer.newLine();
    }

    @Override
    public void run() throws Exception {
        File output = askFile("Select output C file", "Save");
        if (output == null) {
            println("No output file selected.");
            return;
        }

        String target = targetFromProgram(currentProgram);
        try {
            fidService = new FidService();
            fidQueryService = fidService.openFidQueryService(currentProgram.getLanguage(), false);
            fidProgramSeeker =
                fidService.getProgramSeeker(currentProgram, fidQueryService, FID_SCORE_THRESHOLD);
        } catch (Exception e) {
            fidService = null;
            fidQueryService = null;
            fidProgramSeeker = null;
            printerr("FID service unavailable: " + e.getMessage());
        }

        DecompInterface decompiler = new DecompInterface();
        decompiler.toggleCCode(true);
        decompiler.toggleSyntaxTree(false);
        decompiler.openProgram(currentProgram);

        FunctionManager fm = currentProgram.getFunctionManager();
        FunctionIterator functions = fm.getFunctions(true);
        List<Function> functionList = new ArrayList<>();
        while (functions.hasNext()) {
            functionList.add(functions.next());
        }

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(output))) {
            Map<Long, GlobalInfo> globals = collectReferencedGlobals(currentProgram, functionList);
            writeGlobals(writer, target, globals);

            for (Function func : functionList) {
                monitor.checkCanceled();

                if (isLibraryFunction(func)) {
                    writeLibraryEntry(writer, target, func);
                } else {
                    writeDecompiledFunction(writer, target, func, decompiler);
                }
            }
        } finally {
            decompiler.dispose();
            if (fidQueryService != null) {
                fidQueryService.close();
            }
        }

        println("Wrote decompiled output to: " + output.getAbsolutePath());
    }
}
