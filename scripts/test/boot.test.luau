log("--- boot.test ---")

loadscript("test/args.test", "first", "second", "third")
loadscript("test/log.test")
loadscript("test/require.test")
--loadscript("test/Api.test")

Api.test()