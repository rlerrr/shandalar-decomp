//@category Symbol

import ghidra.app.script.GhidraScript;
import ghidra.app.services.DataTypeQueryService;
import ghidra.app.util.parser.FunctionSignatureParser;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.FunctionDefinition;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Function.FunctionUpdateType;
import ghidra.program.model.listing.Parameter;
import ghidra.program.model.listing.Program;
import ghidra.program.model.listing.VariableStorage;
import ghidra.program.model.symbol.SourceType;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.List;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ImportMagicTraceAndParams extends GhidraScript {
    private static final Pattern ADDR_RE = Pattern.compile("\\(([\\da-fA-F]+)\\)");
    private static final Pattern PARAM_REG_RE = Pattern.compile("<([\\w:]+)>");
    private static final Pattern STRIP_REG_RE = Pattern.compile("<[\\w:]+>");
    private static final Pattern RETURN_REG_RE =
        Pattern.compile("<\\s*(eax|edx:eax|rax|xmm0)\\s*>", Pattern.CASE_INSENSITIVE);

    private List<String> extractRegisterHints(String signatureLine) {
        List<String> hints = new ArrayList<>();

        int lparen = signatureLine.indexOf('(');
        int rparen = signatureLine.lastIndexOf(')');
        if (lparen == -1 || rparen == -1 || rparen <= lparen) {
            return hints;
        }

        String params = signatureLine.substring(lparen + 1, rparen).trim();
        if (params.isEmpty()) {
            return hints;
        }

        String[] parts = params.split(",");
        for (String part : parts) {
            String trimmed = part.trim();
            if (trimmed.isEmpty()) {
                hints.add(null);
                continue;
            }

            Matcher m = PARAM_REG_RE.matcher(trimmed);
            if (m.find()) {
                hints.add(m.group(1));
            } else {
                hints.add(null);
            }
        }

        return hints;
    }

    private String stripRegisterAnnotations(String signatureLine) {
        return STRIP_REG_RE.matcher(signatureLine).replaceAll("");
    }

    private String normalizeSignature(String sig) {
        sig = sig.replaceAll("\\bconst\\b\\s*", "");
        sig = sig.replaceAll("\\s+", " ").trim();
        return sig;
    }

    private VariableStorage storageForRegister(String regName) {
        Register reg = currentProgram.getRegister(regName);
        if (reg == null) {
            return null;
        }
        try {
            return new VariableStorage(currentProgram, reg);
        } catch (Exception e) {
            printerr("Failed to create storage for register " + regName + ": " + e.getMessage());
            return null;
        }
    }

    private boolean applyFunctionSignature(Function func, Object signature) {
        try {
            Class<?> cmdClass = Class.forName("ghidra.app.cmd.function.ApplyFunctionSignatureCmd");
            Object cmd = null;

            Constructor<?>[] ctors = cmdClass.getConstructors();
            for (Constructor<?> ctor : ctors) {
                Class<?>[] params = ctor.getParameterTypes();
                if (params.length == 2 &&
                    params[1].isAssignableFrom(signature.getClass())) {
                    if (Function.class.isAssignableFrom(params[0])) {
                        cmd = ctor.newInstance(func, signature);
                        break;
                    }
                    if (Address.class.isAssignableFrom(params[0])) {
                        cmd = ctor.newInstance(func.getEntryPoint(), signature);
                        break;
                    }
                }
                if (params.length == 3 &&
                    params[1].isAssignableFrom(signature.getClass()) &&
                    SourceType.class.isAssignableFrom(params[2])) {
                    if (Function.class.isAssignableFrom(params[0])) {
                        cmd = ctor.newInstance(func, signature, SourceType.USER_DEFINED);
                        break;
                    }
                    if (Address.class.isAssignableFrom(params[0])) {
                        cmd = ctor.newInstance(func.getEntryPoint(), signature, SourceType.USER_DEFINED);
                        break;
                    }
                }
            }

            if (cmd == null) {
                printerr("ApplyFunctionSignatureCmd constructor not found for signature type.");
                return false;
            }

            Method applyTo = null;
            Method[] methods = cmdClass.getMethods();
            for (Method method : methods) {
                if (!"applyTo".equals(method.getName())) {
                    continue;
                }
                Class<?>[] params = method.getParameterTypes();
                if (params.length == 1 && params[0].isAssignableFrom(currentProgram.getClass())) {
                    applyTo = method;
                    break;
                }
            }
            if (applyTo == null) {
                Method[] declared = cmdClass.getDeclaredMethods();
                for (Method method : declared) {
                    if (!"applyTo".equals(method.getName())) {
                        continue;
                    }
                    Class<?>[] params = method.getParameterTypes();
                    if (params.length == 1 && params[0].isAssignableFrom(currentProgram.getClass())) {
                        applyTo = method;
                        applyTo.setAccessible(true);
                        break;
                    }
                }
            }
            if (applyTo == null) {
                printerr("ApplyFunctionSignatureCmd applyTo method not found for program type.");
                return false;
            }

            Object result = applyTo.invoke(cmd, currentProgram);
            return result == null || Boolean.TRUE.equals(result);
        } catch (Exception e) {
            printerr("ApplyFunctionSignatureCmd failed: " + e.getMessage());
            return false;
        }
    }

    private boolean replaceParametersFallback(Function func, FunctionDefinition signature) {
        try {
            Class<?> paramImplClass = Class.forName("ghidra.program.model.listing.ParameterImpl");
            Field unassignedField = null;
            try {
                unassignedField = VariableStorage.class.getField("UNASSIGNED_STORAGE");
            } catch (NoSuchFieldException ignored) {
                // Some versions omit UNASSIGNED_STORAGE; use null storage fallback.
            }

            Object unassignedStorage = (unassignedField != null) ? unassignedField.get(null) : null;
            List<Object> params = new ArrayList<>();
            for (ghidra.program.model.data.ParameterDefinition def : signature.getArguments()) {
                Object param = null;
                try {
                    Constructor<?> ctor = paramImplClass.getConstructor(
                        String.class, DataType.class, VariableStorage.class, Program.class
                    );
                    param = ctor.newInstance(def.getName(), def.getDataType(), unassignedStorage, currentProgram);
                } catch (NoSuchMethodException ignored) {
                    Constructor<?> ctor = paramImplClass.getConstructor(
                        String.class, DataType.class, Program.class
                    );
                    param = ctor.newInstance(def.getName(), def.getDataType(), currentProgram);
                }
                params.add(param);
            }

            Method replace = func.getClass().getMethod(
                "replaceParameters", List.class, FunctionUpdateType.class, boolean.class, SourceType.class
            );
            replace.invoke(func, params, FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.USER_DEFINED);
            return true;
        } catch (Exception e) {
            printerr("Fallback parameter replace failed: " + e.getMessage());
            return false;
        }
    }

    private boolean replaceParametersWithStorage(
        Function func,
        FunctionDefinition signature,
        List<String> regHints
    ) {
        try {
            Class<?> paramImplClass = Class.forName("ghidra.program.model.listing.ParameterImpl");
            Field unassignedField = null;
            try {
                unassignedField = VariableStorage.class.getField("UNASSIGNED_STORAGE");
            } catch (NoSuchFieldException ignored) {
                // Some versions omit UNASSIGNED_STORAGE; use null storage fallback.
            }

            Object unassignedStorage = (unassignedField != null) ? unassignedField.get(null) : null;
            List<Object> params = new ArrayList<>();
            int index = 0;
            for (ghidra.program.model.data.ParameterDefinition def : signature.getArguments()) {
                VariableStorage storage = null;
                if (index < regHints.size()) {
                    String regName = regHints.get(index);
                    if (regName != null) {
                        storage = storageForRegister(regName);
                    }
                }
                Object storageObj = (storage != null) ? storage : unassignedStorage;

                Object param = null;
                try {
                    Constructor<?> ctor = paramImplClass.getConstructor(
                        String.class, DataType.class, VariableStorage.class, Program.class
                    );
                    param = ctor.newInstance(def.getName(), def.getDataType(), storageObj, currentProgram);
                } catch (NoSuchMethodException ignored) {
                    Constructor<?> ctor = paramImplClass.getConstructor(
                        String.class, DataType.class, Program.class
                    );
                    param = ctor.newInstance(def.getName(), def.getDataType(), currentProgram);
                }

                params.add(param);
                index++;
            }

            Method replace = func.getClass().getMethod(
                "replaceParameters", List.class, FunctionUpdateType.class, boolean.class, SourceType.class
            );
            FunctionUpdateType updateType = FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS;
            try {
                updateType = FunctionUpdateType.valueOf("CUSTOM_STORAGE");
            } catch (Exception ignored) {
                // Fall back to dynamic storage when custom isn't available.
            }
            replace.invoke(func, params, updateType, true, SourceType.USER_DEFINED);
            return true;
        } catch (Exception e) {
            printerr("Parameter replace with storage failed: " + e.getMessage());
            return false;
        }
    }

    private void setParameterStorage(Parameter param, VariableStorage storage) {
        if (tryInvokeStorageMethod(param, "setStorage", storage)) {
            return;
        }
        if (tryInvokeStorageMethod(param, "setVariableStorage", storage)) {
            return;
        }
        printerr("Failed to apply register storage: no compatible method on " + param.getClass().getName());
    }

    private boolean tryInvokeStorageMethod(Parameter param, String methodName, VariableStorage storage) {
        Method target = null;
        try {
            target = param.getClass().getMethod(methodName, VariableStorage.class);
        } catch (NoSuchMethodException ignored) {
            // Fall back to declared methods.
        }
        if (target == null) {
            Method[] declared = param.getClass().getDeclaredMethods();
            for (Method method : declared) {
                if (!methodName.equals(method.getName())) {
                    continue;
                }
                Class<?>[] params = method.getParameterTypes();
                if (params.length == 1 && params[0].isAssignableFrom(storage.getClass())) {
                    target = method;
                    target.setAccessible(true);
                    break;
                }
            }
        }
        if (target == null) {
            return false;
        }
        try {
            target.invoke(param, storage);
            return true;
        } catch (Exception e) {
            printerr("Failed to apply register storage: " + e.getMessage());
            return true;
        }
    }

    private boolean hasExplicitReturnType(String sig) {
        int lparen = sig.indexOf('(');
        if (lparen <= 0) {
            return false;
        }
        String before = sig.substring(0, lparen).trim();
        return before.contains(" ") || before.contains("\t");
    }

    @Override
    public void run() throws Exception {
        FunctionSignatureParser parser = new FunctionSignatureParser(
            currentProgram.getDataTypeManager(),
            state.getTool().getService(DataTypeQueryService.class)
        );

        File symbolFile = askFile("Select symbol file", "Import");
        Long currentAddr = null;

        try (BufferedReader reader = new BufferedReader(new FileReader(symbolFile))) {
            String line;
            while ((line = reader.readLine()) != null) {
                monitor.checkCanceled();

                line = line.trim();
                if (line.isEmpty()) {
                    continue;
                }

                Matcher m = ADDR_RE.matcher(line);
                if (m.find()) {
                    currentAddr = Long.parseLong(m.group(1), 16);
                    continue;
                }

                if (currentAddr == null || line.indexOf('(') == -1) {
                    continue;
                }

                Address entry = toAddr(currentAddr);

                List<String> regHints = extractRegisterHints(line);
                boolean hasRegParams = false;
                for (String hint : regHints) {
                    if (hint != null) {
                        hasRegParams = true;
                        break;
                    }
                }

                String cleanSig = normalizeSignature(stripRegisterAnnotations(line));
                boolean hasReturnRegister = RETURN_REG_RE.matcher(line).find();

                FunctionDefinition fdef;
                try {
                    String parseSig = cleanSig;
                    if (!hasExplicitReturnType(cleanSig)) {
                        parseSig = (hasReturnRegister ? "int " : "void ") + cleanSig;
                    }
                    fdef = (FunctionDefinition) parser.parse(null, parseSig);
                } catch (Exception e) {
                    printerr("Signature parse failed:");
                    printerr(" " + cleanSig);
                    printerr(" " + e.getMessage());
                    currentAddr = null;
                    continue;
                }

                try {
                    clearListing(entry);
                } catch (Exception e) {
                    // Ignore; some addresses may not be clearable.
                }
                try {
                    disassemble(entry);
                } catch (Exception e) {
                    // Ignore; disassembly can fail if already code.
                }

                Function func = getFunctionAt(entry);
                if (func == null) {
                    func = createFunction(entry, fdef.getName());
                }
                if (func == null) {
                    printerr("Failed to create function at " + entry);
                    currentAddr = null;
                    continue;
                }

                func.setName(fdef.getName(), SourceType.USER_DEFINED);
				
                try {
                    func.setCallingConvention(hasRegParams ? "unknown" : "__cdecl");
                } catch (Exception e) {
                    printerr("Failed to set calling convention: " + func.getName() + " " + e.getMessage());
                }

                boolean applied = applyFunctionSignature(func, fdef);
                if (hasRegParams) {
                    replaceParametersWithStorage(func, fdef, regHints);
                } else if (!applied) {
                    replaceParametersFallback(func, fdef);
                }

                //func.setReturnType(fdef.getReturnType(), SourceType.USER_DEFINED);

                currentAddr = null;
            }
        }

        println("Hybrid signature import complete.");
    }
}
