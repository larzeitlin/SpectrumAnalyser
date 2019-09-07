** TODO

- Dynamically capture format, number of channels, samplecount on song load and use to reset buffer lengths etc.
- Pull out helper functions (into a header maybe) - Han window, normalize
- reduce data duplication while processing in processAudioBuffer (some of this moves to the audioProcessor class)
- Present visual betterer. for example:
-- with an axis, logarithmically, and more centered
-- interpolation to smooth movement
-- history (trails)
-- lighting
-- perspective

