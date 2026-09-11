// Export Ghidra function starts, names, and contiguous extents as CSV.
// @category Shandalar

import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

public class ExportFunctions extends GhidraScript {
    private String csv(String value) {
        return "\"" + value.replace("\"", "\"\"") + "\"";
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) {
            throw new IllegalArgumentException("Expected output CSV path");
        }

        PrintWriter out = new PrintWriter(new File(args[0]), "UTF-8");
        try {
            out.println("address,name,orig_size");
            FunctionIterator functions =
                currentProgram.getFunctionManager().getFunctions(true);
            while (functions.hasNext() && !monitor.isCancelled()) {
                Function function = functions.next();
                out.printf(
                    "%08x,%s,%d%n",
                    function.getEntryPoint().getOffset(),
                    csv(function.getName()),
                    function.getBody().getMaxAddress().subtract(
                        function.getEntryPoint()
                    ) + 1
                );
            }
        } finally {
            out.close();
        }
    }
}
