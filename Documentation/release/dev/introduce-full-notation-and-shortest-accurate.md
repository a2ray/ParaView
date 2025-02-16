## New real number display settings

Introduce two real number display settings to control how the real number are displayed in ParaView interface.

1. FullNotation

A new notation has been added, FullNotation.
It Always use full notation instead of showing a simplified text. This restore the behavior of ParaView 5.7 before the introduction of the real number display setting

![Illustration of using the full notation](introduce-full-notation-and-shortest-accurate_full.png)

2. Shortest Accurate Precision

Instead of setting a precision, it is now possible to use the shortest accurate precision, that relies on a Qt mechanism to find an such a number representation

![Illustration of using the shortest accurate precision](introduce-full-notation-and-shortest-accurate_short.png)

These settings have been mirrored for the timestep values as well.
