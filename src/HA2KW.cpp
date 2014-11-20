#include "stdafx.h"

#if defined _HA2KW_H_
HA2KW::HA2KW(void)
{
    int i = 0;
        
    //общая часть
    for(i = 0;i < 128;i++)
    {
	    alt2_windows[i] = i;
	    windows_alt2[i] = i;

	    alt2_koi8[i] = i;
	    koi8_alt2[i] = i;
    }
    for(i = 128;i < 256;i++)
    {
	    alt2_windows[i] = 6;
	    windows_alt2[i] = 6;

	    alt2_koi8[i] = 6;
	    koi8_alt2[i] = 6;
    }
    /////////////////////////////////
    //for alternative2(DOS) and windows
    for(i = 128;i < 176;i++)
    {
	    alt2_windows[i] = i + 64;
    }
    for(i = 192;i < 240;i++)
    {
	    windows_alt2[i] = i - 64;
    }

    for(i = 224;i < 240;i++)
    {
	    alt2_windows[i] = i + 16;
    }
    for(i = 240;i < 256;i++)
    {
	    windows_alt2[i] = i - 16;
    }

    alt2_windows[240] = 168;	windows_alt2[168] = 240;
    alt2_windows[241] = 184;	windows_alt2[184] = 241;
    alt2_windows[242] = 170;	windows_alt2[168] = 242;
    alt2_windows[243] = 186;	windows_alt2[184] = 243;
    alt2_windows[244] = 175;	windows_alt2[175] = 244;
    alt2_windows[245] = 191;	windows_alt2[191] = 245;
    alt2_windows[246] = 161;	windows_alt2[161] = 246;
    alt2_windows[247] = 162;	windows_alt2[162] = 247;
    alt2_windows[248] = 176;	windows_alt2[176] = 248;
    alt2_windows[249] = 183;	windows_alt2[183] = 249;
    alt2_windows[250] = 183;	windows_alt2[183] = 250;
    alt2_windows[252] = 184;	windows_alt2[184] = 252;
    alt2_windows[253] = 164;	windows_alt2[164] = 253;
    alt2_windows[255] = 160;	windows_alt2[160] = 255;
    
    /////////////////////////////////
    //for alternative2(DOS) and koi8
    alt2_koi8[128] = 225;	koi8_alt2[225] = 128;
    alt2_koi8[129] = 226;	koi8_alt2[226] = 129;
    alt2_koi8[130] = 247;	koi8_alt2[247] = 130;
    alt2_koi8[131] = 231;	koi8_alt2[231] = 131;
    alt2_koi8[132] = 228;	koi8_alt2[228] = 132;
    alt2_koi8[133] = 229;	koi8_alt2[229] = 133;
    alt2_koi8[134] = 246;	koi8_alt2[246] = 134;
    alt2_koi8[135] = 250;	koi8_alt2[250] = 135;
    alt2_koi8[136] = 233;	koi8_alt2[233] = 136;
    alt2_koi8[137] = 234;	koi8_alt2[234] = 137;
    alt2_koi8[138] = 235;	koi8_alt2[235] = 138;
    alt2_koi8[139] = 236;	koi8_alt2[236] = 139;
    alt2_koi8[140] = 237;	koi8_alt2[237] = 140;
    alt2_koi8[141] = 238;	koi8_alt2[238] = 141;
    alt2_koi8[142] = 239;	koi8_alt2[239] = 142;
    alt2_koi8[143] = 240;	koi8_alt2[240] = 143;
    alt2_koi8[144] = 242;	koi8_alt2[242] = 144;
    alt2_koi8[145] = 243;	koi8_alt2[243] = 145;
    alt2_koi8[146] = 244;	koi8_alt2[244] = 146;
    alt2_koi8[147] = 245;	koi8_alt2[245] = 147;
    alt2_koi8[148] = 230;	koi8_alt2[230] = 148;
    alt2_koi8[149] = 232;	koi8_alt2[232] = 149;
    alt2_koi8[150] = 227;	koi8_alt2[227] = 150;
    alt2_koi8[151] = 254;	koi8_alt2[254] = 151;
    alt2_koi8[152] = 251;	koi8_alt2[251] = 152;
    alt2_koi8[153] = 253;	koi8_alt2[253] = 153;
    alt2_koi8[154] = 255;	koi8_alt2[255] = 154;
    alt2_koi8[155] = 249;	koi8_alt2[249] = 155;
    alt2_koi8[156] = 248;	koi8_alt2[248] = 156;
    alt2_koi8[157] = 252;	koi8_alt2[252] = 157;
    alt2_koi8[158] = 224;	koi8_alt2[224] = 158;
    alt2_koi8[159] = 241;	koi8_alt2[241] = 159;

    for(i = 160;i < 176;i++)
    {
	    alt2_koi8[i] = alt2_koi8[i - 32] - 32;
	    koi8_alt2[alt2_koi8[i - 32] - 32] = i;
    }

    for(i = 224;i < 240;i++)
    {
	    alt2_koi8[i] = alt2_koi8[i - 80] - 32;
	    koi8_alt2[alt2_koi8[i - 80] - 32] = i;
    }

    alt2_koi8[184] = 144;	koi8_alt2[144] = 184;
    alt2_koi8[185] = 145;	koi8_alt2[145] = 185;
    alt2_koi8[186] = 146;	koi8_alt2[146] = 186;
    alt2_koi8[187] = 129;	koi8_alt2[129] = 187;
    alt2_koi8[188] = 135;	koi8_alt2[135] = 188;
    alt2_koi8[189] = 178;	koi8_alt2[178] = 189;
    alt2_koi8[190] = 167;	koi8_alt2[167] = 190;
    alt2_koi8[191] = 166;	koi8_alt2[166] = 191;

    alt2_koi8[240] = 179;	koi8_alt2[179] = 240;
    alt2_koi8[241] = 163;	koi8_alt2[163] = 241;
}

HA2KW::~HA2KW(void)
{
}

void HA2KW::Code(char *buffer, int cpin, int cpout)
{
    int j = 0;
	while (! buffer [j] == 0)
	{
		switch(cpin)
		{
			case CODE_RUSSIAN_ALTERNATIVE2:
				switch(cpout)
				{
					case CODE_RUSSIAN_ALTERNATIVE2:
						//ничего не делаем
						break;
					case CODE_RUSSIAN_KOI8:
						if(alt2_koi8 [(unsigned char) buffer [j]] > 127)
						{
							buffer[j] = alt2_koi8 [(unsigned char) buffer [j]];
						}
						break;
					case CODE_RUSSIAN_WINDOWS://самый первый
						if(alt2_windows [(unsigned char) buffer [j]] > 127)
						{
							buffer[j] = alt2_windows [(unsigned char) buffer [j]];
						}
						break;
				}
				break;
			case CODE_RUSSIAN_KOI8:
				switch(cpout)
				{
					case CODE_RUSSIAN_ALTERNATIVE2:
						if(koi8_alt2 [(unsigned char) buffer [j]] > 127)
						{
							buffer[j] = koi8_alt2 [(unsigned char) buffer [j]];
						}
						break;
					case CODE_RUSSIAN_KOI8:
						//ничего не делаем
						break;
					case CODE_RUSSIAN_WINDOWS:
						if(koi8_alt2 [(unsigned char) buffer [j]] > 127)
						{
							//так как нет таких таблиц, то
							//сначала перекодируем в рускую(DOS)
							buffer[j] = koi8_alt2 [(unsigned char) buffer [j]];
							//, а затем в WINDOWS
							buffer[j] = alt2_windows [(unsigned char) buffer [j]];
						}
						break;
				}
				break;
			case CODE_RUSSIAN_WINDOWS:
				switch(cpout)
				{
					case CODE_RUSSIAN_ALTERNATIVE2:
						if(windows_alt2 [(unsigned char) buffer [j]] > 127)
						{
							buffer[j] = windows_alt2 [(unsigned char) buffer [j]];
						}
						break;
					case CODE_RUSSIAN_KOI8:
						if(windows_alt2 [(unsigned char) buffer [j]] > 127)
						{
							//так как нет таких таблиц, то
							//сначала перекодируем в рускую(DOS)
							buffer[j] = windows_alt2 [(unsigned char) buffer [j]];
							//, а затем в KOI8
							buffer[j] = alt2_koi8 [(unsigned char) buffer [j]];
						}
						break;
					case CODE_RUSSIAN_WINDOWS:
						//ничего не делаем
						break;
				}
				break;
		}
		
		j ++;
	}
}
#else
#endif