# display-bitmap
Example of GUI window that show a bitmap.

The bitmap is not loaded from disk, it is embedded in the application using ```bin2hdr```.



# bin2hdr
Convert a file to an C header file with array of binary data.

## Usage example
```
C:\bin2hdr.exe sample.bmp
Input file      'sample.bmp'
Output file     'sample.bmp.h'
21834 bytes written.
```

## Example of generated code

```C
#ifndef BIN2HDR_INCL_H
#define BIN2HDR_INCL_H
#include <stdint.h> /* uint8_t declaration */
const uint8_t BIN_DATA [3] = {
0x42, 0x4D, 0x66, };
#endif /*BIN2HDR_INCL_H*/
```
