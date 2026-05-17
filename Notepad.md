# Notepad Project Notes

This file describes the optional features I implemented in the Notepad project, why I chose them, and how they work at a high level.

## Overview

The base version of the project already included the main text editor, file operations, editing tools, text transformations, formatting actions, find and replace, word frequency analysis, spell checking, and a status bar.

To make the editor more practical and comfortable to use, I added five optional features:

1. Cursor line and column indicator
2. Font dialog
3. Text color picker
4. Zoom controls
5. Recent files

I chose these features because they improve everyday usability. Together, they make the application feel closer to a small word processor instead of just a minimal text editor.

## 1. Cursor Line and Column Indicator

### Why I added it

When a user is writing or editing text, it is helpful to know the exact cursor position. This is especially useful when working with multi-line text, correcting formatting, or navigating long content.

### How it works for the user

The current line number and column number are shown in the status bar at the bottom of the window. As the cursor moves, the displayed position updates automatically.

Example:

`Line: 3, Col: 15`

This means the cursor is currently on line 3, character 15 of that line.

### How it is implemented

The editor listens for cursor movement using the `cursorPositionChanged` signal from `QTextEdit`. Every time the cursor changes position, the program:

- gets the current text cursor,
- reads the current block number as the line,
- reads the position inside the block as the column,
- adds `1` to both values so the numbering starts from `1` instead of `0`,
- updates the label in the status bar.

This makes the indicator live and always accurate while the user is typing or moving through the document.

## 2. Font Dialog

### Why I added it

Users often want to change the appearance of text to make it clearer or more visually organized. A font dialog gives a simple and familiar way to choose a different font family, size, or style.

### How it works for the user

The feature is available through:

`Format -> Font...`

When the user opens it, a standard Qt font selection window appears. After choosing a font and confirming, the selected font is applied.

Behavior:

- if text is selected, the chosen font is applied to that selection;
- if nothing is selected, the chosen font is applied more broadly to the document/current typing format.

This makes the feature convenient in both cases: editing a small part of text or changing the general appearance of the document.

### How it is implemented

The program opens `QFontDialog::getFont(...)` and checks whether the user pressed OK. If the selection is confirmed, the chosen font is placed into a `QTextCharFormat`, and that format is applied through a helper function.

I used a separate formatting helper so the logic stays clean and can work consistently with both selected text and document-wide formatting behavior.

## 3. Text Color Picker

### Why I added it

Text color helps emphasize important words, headings, or notes. It gives the user another simple formatting option and makes the editor more flexible for visually structured text.

### How it works for the user

The feature is available through:

`Format -> Text Color...`

When selected, the application opens a standard color picker. The user chooses a color, and that color is applied to the selected text or to the current text formatting if no selection is active.

This can be used, for example, to:

- highlight important parts of a note,
- mark sections with different meanings,
- make headings stand out more clearly.

### How it is implemented

The program opens `QColorDialog::getColor(...)`. If the chosen color is valid, it creates a `QTextCharFormat`, sets the foreground color, and applies it through the same formatting flow used for other rich-text styling features.

Using Qt's built-in color dialog keeps the interface simple and familiar.

## 4. Zoom Controls

### Why I added it

Different users prefer different reading sizes, and text may look too small or too large depending on the screen. Zoom controls make the application more accessible and more comfortable to use.

### How it works for the user

The feature is available through the `View` menu:

- `View -> Zoom In`
- `View -> Zoom Out`
- `View -> Reset Zoom`

Keyboard shortcuts are also connected for faster use.

This allows the user to quickly enlarge the text for readability, reduce it to see more content, or return to the default scale at any time.

### How it is implemented

The editor uses Qt's built-in zoom support:

- `zoomIn()` increases the scale,
- `zoomOut()` decreases the scale.

I also store the current zoom offset in a variable called `zoomLevel_`. This is important for the reset behavior. Instead of guessing the default size, the program keeps track of how many zoom steps were applied and then reverses them when `Reset Zoom` is used.

That makes reset reliable whether the user zoomed in multiple times or zoomed out multiple times.

## 5. Recent Files

### Why I added it

Opening the same files repeatedly can be inconvenient if the user must browse for them every time. A recent files list makes reopening documents much faster and improves the overall workflow.

### How it works for the user

The feature is available through:

`File -> Recent Files`

This submenu shows up to five recently opened or saved files. When the user clicks one of them, the file opens directly.

Additional behavior:

- duplicate entries are avoided,
- the newest file is moved to the top,
- missing files are filtered out when the list is loaded,
- the menu includes a `Clear Recent Files` action.

If there are no saved recent files, the menu shows `(Empty)`.

### How it is implemented

I store recent file paths in a `QStringList` and limit the list to five entries. Every time a file is opened successfully or saved, the path is added to the list.

To keep the feature persistent between application launches, the project uses `QSettings`. That means the recent file list is saved automatically and restored the next time the program starts.

When rebuilding the menu, the application creates one action per file and attaches the file path to that action. Clicking the action calls a slot that reads the stored path and opens the corresponding file.

## Final Notes

These optional features were selected to improve usability rather than add unnecessary complexity. They are easy for the user to discover, fit naturally into the existing menu structure, and integrate well with the rest of the editor.

In summary:

- the cursor position indicator improves navigation,
- the font dialog improves text appearance control,
- the text color picker adds visual emphasis,
- the zoom controls improve readability,
- the recent files menu improves speed and convenience.

Together, these additions make the Notepad project more polished, practical, and user-friendly.
