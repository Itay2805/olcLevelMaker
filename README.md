# olcLevelMaker

This is a tool for editing levels and maps for [olcConsoleGameEngine]()

# Features

* 16x16 Spritesheet
* Set tiles as solid
* Flood Fill
* Save/Load levels
* Export level as a sprite

# Controls

## General Controls

- Moving the map around: `WSAD`
- Changing menu: `T`
- Enabling/Disabling flood mode: `F`
- Enabling/Disabling grid mode: `G`
- Quick Save: `CTRL + S`
- Quick Load: `CTRL + L`

## Tiles Menu
- Setting Hovered Tile: `left click`
- Selecting Hovered Tile: `right click`
- Quick Flood Fill: `CTRL + left click`
- Changing tiles page: `left arrow` and `right arrow`
- Selection rect: hold `CTRL` and click on the first and second position, then release `CTRL`

### In Selection Rect
while selection rect is on you can not draw outside of it. To cancel it press `ESC`.

if you press `BACKSPACE` it will fill the selection rect with the selected tile.

## Tile Meta Menu
First you need to select which meta tool you want to use

### Solid
- Set solid: `left click`
- Unset solid: `right click`
- Quick Flood Fill: `CTRL + left click`