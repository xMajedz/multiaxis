log("--- Api.test ---")

function Api.prototype.TestValid(first, second, third)
   print(`Api.prototype.TestValid -> {first}, {second}, {third}`)
end

log(Api.prototype.TestValid)

function Api.prototype.TestInvalid(first, second, third)
   assert(false, `Api.prototype.TestInvalid -> {first}, {second}, {third}`)
end

log(Api.prototype.TestInvalid)

Api.prototype("first", "second", "third")

local message = ""

function Api.Console.test(msg)
   message = `{msg} PASS`
   Api.Console.test = nil
end

log("Api.Console.test")
log(message)

log("PASS")
