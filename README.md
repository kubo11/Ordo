# Ordo
Ordo is a wayland compositor based on Mir.

## Running

To use custom config either place it in your ~/.config directory or add its location to XDG_CONFIG_DIRS variable. eg.
```
WAYLAND_DISPLAY=wayland-99 XDG_CONFIG_DIRS="Ordo:$XDG_CONFIG_DIRS" Ordo/build/src/ordo
```