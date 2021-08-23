package Frames;

public class CRC16 {
	public static int crc_16_CCITT_False(byte[] bytes) {
		int len = bytes.length;
		int crc = 0xffff;
		int polynoimial = 0x1021;
		for (int i = 0; i < len; i++) {
			byte b = bytes[i];
			for (int j = 0; j < 8; j++) {
				boolean bit = ((b >> (7 - i) & 1) == 1);
				boolean c15 = ((crc >> 15 & 1) == 1);
				crc <<= 1;
				if (c15 ^ bit)
					crc ^= polynoimial;
			}
		}
		crc &= 0xffff;
		return crc;
	}

	public static boolean crc_16_CCITT_Check(int crc, byte[] bytes) {
		int checkcode = crc_16_CCITT_False(bytes);
		return checkcode == crc;
	}
}
