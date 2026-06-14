log("--- require.test ---")

require()

require(nil)

log(require("test/return_table.test"))

log(require("test/return_table.test")[1])

log("PASS")