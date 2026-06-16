log("--- Api.test ---")

function Api.prototype.test(first, second, third)
    log(`Api.prototype.test -> {first}, {second}, {third}`)
end

log(Api.prototype.test)

Api.prototype("first", "second", "third")

local message = ""

function Api.Console.test(msg)    
	message = `{msg} PASS`
	Api.Console.test = nil
end

log("Api.Console.test")
log(message)

log("PASS")