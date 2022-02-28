scriptName TextInputUtil hidden
{Utility for prompting for text from within Skyrim.

- When playing Skyrim VR, the VR keyboard is used.
- When playing other versions of Skyrim, UI Extensions must be installed.

### Usage

```psc
string text = TextInputUtil.GetText()
```

You can optionally provide the default text shown in the text input:

```psc
string text = TextInputUtil.GetText("This is the starting text")
```
}

string function GetText(string startingText = "") global
{Usage: `string text = TextInputUtil.GetText()`
    
You can optionally provide the default text shown in the text input:

```psc
string text = TextInputUtil.GetText("This is the starting text")
```
}
    if Game.GetModByName("SkyrimVR.esm") == 255 ; If it's not Skyrim VR
        UITextEntryMenu textEntry = UIExtensions.GetMenu("UITextEntryMenu") as UITextEntryMenu
        if textEntry
            if startingText
                textEntry.SetPropertyString("text", startingText)
            endIf
            textEntry.OpenMenu()
            return textEntry.GetResultString()
        else
            return ""
        endIf
    else
        return VRKeyboard.GetKeyboardInput(startingText)
    endIf
endFunction
