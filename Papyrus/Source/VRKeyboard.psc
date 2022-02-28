scriptName VRKeyboard hidden
{Display the VR keyboard in Skyrim using the `VRKeyboard.GetKeyboardInput()` function.

### Usage

```psc
string text = VRKeyboard.GetKeyboardInput()
```

- If you want to get text input in _any_ version of Skyrim, see `TextInputUtil.GetText()`.
- Using `VRKeyboard` directly provides additional functionality.

You can optionally provide the default text shown in the text input:

```psc
string text = VRKeyboard.GetKeyboardInput("This is the starting text")
```
}

; string function GetKeyboardInputAsync(string callback, string startingText = "", string lineMode = "single", string style = "normal") global native

bool function OpenKeyboard(string startingText = "") global native
{Opens the VR Keyboard. Once open, use `PollForFinishedKeyboardEntry` to get the entered text.

> _Note: you probably don't want to use this directly. Use `GetKeyboardInput` instead._}

string function PollForFinishedKeyboardEntry() global native
{Polls for the VR keyboard text to be finished and returns the entered text.

> _Note: you probably don't want to use this directly. Use `GetKeyboardInput` instead._}

string function GetKeyboardInput(string startingText = "", float waitInterval = 0.1) global
{Display the VR keyboard in Skyrim using the `VRKeyboard.GetKeyboardInput()` function.

### Usage

```psc
string text = VRKeyboard.GetKeyboardInput()
```

- If you want to get text input in _any_ version of Skyrim, see `TextInputUtil.GetText()`.
- Using `VRKeyboard` directly provides additional functionality.

You can optionally provide the default text shown in the text input:

```psc
string text = VRKeyboard.GetKeyboardInput("This is the starting text")
```

- The optional `waitInterval` property defines the number of seconds
  used to wait when polling for the keyboard entry to be complete.
  The default is 0.1 seconds (every 100ms).
}
    OpenKeyboard(startingText)
    string resultText
    while ! resultText
        resultText = PollForFinishedKeyboardEntry()
        if ! resultText
            Utility.WaitMenuMode(waitInterval)
        endIf
    endWhile
    return resultText
endFunction
