Please follow the instructions below for the program:

When assigning the port on the client please type in 12345. This is the port on the server side.
The following filters are allowed:
Rotation, box blur, sharpening image, converting image to greyscale, flipping image, adjusting brightness, adjusting contrast, adjusting gamma, resizing image

When running the client user input must be the following: ClientSide.exe "IP:Port" "imageName.jpg" "filterType" "numericValue for filter (if required)" "additional arguments for filter (if required)"
The input for each filter (arguments 4-6). where " " is used, this means a custom value specified by the user.:
Rotation: rotation "angle" "clockwise/counterclockwise" (angle required to execute)
Box: box
Sharpening: sharpening
Greyscale: greyscale
Flip: flip "0, 1 or -1" (0 means flip horizontally. 1 means flip vertical. -1 means flip both axis. If no number is given 0 is used by default)
Brightness: brightness "value" (if no value is given it is by default 0)
Contrast: contrast "value" (if no value is given it is by default 0)
Gamma: gamma "value" (if no value is given it is by default 0)
Resize: size "width" "height" (all 3 arguments must be given to execute)