log("--- boot.test ---")

runscript("test/args.test", "first", "second", "third")
runscript("test/log.test")
runscript("test/require.test")
runscript("test/Api.test")