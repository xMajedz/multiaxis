log("--- args.test ---")

local args = {...}

log(`argc = {#args}`)

for i, arg in args do log(`args[{i}] = {arg}`) end