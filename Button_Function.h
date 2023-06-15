#ifndef __Button_Function_H__
#define __Button_Function_H__

void ReadKey(unsigned char* KeyRam);						
unsigned char GetKey(unsigned char* KeyCode,unsigned char* KeyRam,unsigned char* PreKeyRam,unsigned char* KeyCnt,unsigned char* NoKeyCnt);

#endif