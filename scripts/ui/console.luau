local ui = require "@uielement"

local console = {}

local function showMessages(viewElement, messages)
    local maxMessages = 40
    local max = ((#messages > maxMessages) and maxMessages) or #messages
    local d = ((#messages > maxMessages) and (#messages - maxMessages)) or 0
    
    for i = 1, #messages - d do
        local elem = viewElement:addChild{
	    rec = {0, -20 * (max - i + 1), ui.WIN_W - 20, 20},
	    bgColor = ui.UICOLORBLANK,
        }
       
        elem:addAdaptedText(messages[i + d], 5, 0, 0, 0)
    end
end

function console.makeTextfield(textfieldString)
    local textfieldBuffer = buffer.create(256)

    local textfieldIndex = #textfieldString

    buffer.writestring(textfieldBuffer, 0, textfieldString)

    local viewElement = ui.new{
        rec = {10, ui.WIN_H - 25, ui.WIN_W - 20, 20},

        bgColor = ui.UICOLORBLANK,
        hoverColor = ui.UICOLORBLANK,
        pressedColor = ui.UICOLORBLUE,
    
        borderColor = ui.UICOLORWHITE,
        borderHoverColor = ui.UICOLORBLUE,
     
        interactive = true,
	
	keyboard = true,
    }

    local messages = {}
    
    local messageHolder = ui.new{
        --rec = {10, 70, ui.WIN_W - 20, 20 * 40}, 
        --rec = {10, ui.WIN_H - 50, ui.WIN_W - 20, 20},
        rec = {10, 70, ui.WIN_W - 20, 20},
        bgColor = ui.UICOLORBLANK,
	--borderColor = ui.UICOLORWHITE,
    }
    
    function messageHolder.customDisplay()
        local maxMessages = 40
        local max = ((#messages > maxMessages) and maxMessages) or #messages
        local d = ((#messages > maxMessages) and (#messages - maxMessages)) or 0
        for i = 1, #messages - d do
	   messageHolder:uiText(messages[i + d], 5, maxMessages * 20 - 20 * (max - i + 1), nil, 0, nil, 20, ui.UICOLORWHTE)
        end
    end

    function Api.Console.Console(message)
        --messageHolder:kill(true)
        table.insert(messages, message)
        messageHolder:show()
	--showMessages(messageHolder, messages)
    end

    viewElement:hide()

    function viewElement.customDisplay()
        local color = (viewElement.hoverState and ui.UICOLORBLUE) or ui.UICOLORWHITE
        local text = buffer.readstring(textfieldBuffer, 0, textfieldIndex)
	viewElement:uiText(text .. "_", 5, 0, nil, 0, nil, 20, color)
    end
    
    function viewElement.enterAction()
        messageHolder:hide()	    
        if viewElement.displayed then
	    if textfieldIndex > 0 then
	       local text = buffer.readstring(textfieldBuffer, 0, textfieldIndex)
	       textfieldIndex = 0
	       table.insert(messages, text)
	       messageHolder:show()
	       --showMessages(messageHolder, messages)
	    end
	    viewElement:hide()
	else
	    messageHolder:show()
	    --showMessages(messageHolder, messages)
	    viewElement:show()
       	end
    end
    
    function viewElement.keyPressed(key, keyCode)
        if keyCode == 259 and textfieldIndex > 0 then
	    textfieldIndex -= 1
	end
        if key > 0 and textfieldIndex < 256 and viewElement.displayed then
	    buffer.writeu8(textfieldBuffer, textfieldIndex, key)
            textfieldIndex += 1
        end
    end

    return viewElement
end

return console
