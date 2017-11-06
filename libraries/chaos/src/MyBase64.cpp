#include <chaos/MyBase64.h>
#include <chaos/SparseWriteBuffer.h>

namespace chaos
{

	static bool MyBase64::IsBase64(unsigned char c) 
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	char const * MyBase64::base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	// XXX : explanation
	//       we split input into groups of 3 bytes [0-255]
	//       we can see 3 bytes as
	//          => 3 x 8 = 24 bits (normal)
	//          => 4 x 6 = 24 bits (encoded) 
	//
	//       6 bits = 64 values. we use a map with 64 alpha numerical characters
	//	
	void MyBase64::EncodeBuffer(unsigned char const * char_array_3, unsigned char * char_array_4)
	{
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;
	}

	std::string MyBase64::Encode(Buffer<char> const & src)
	{
		static const int CHUNK_SIZE = 1024 * 16;

		std::string result;
		result.reserve((src.bufsize * 4) / 3); // there is a 4/3 ratio between input and output
	
		int tmp = 0;

		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		for (size_t i = 0 ; i < src.bufsize ; ++i)
		{
			char_array_3[tmp++] = src.data[i];
			if (tmp == 3) 
			{			
				EncodeBuffer(char_array_3, char_array_4);
				for (int k = 0 ; k < 4 ; ++k)
					result += base64_chars[char_array_4[k]]; // flush the buffer
				tmp = 0;
			}
		}

		if (tmp > 0) // there are still some bytes to flush
		{
			for(int k = tmp ; k < 3; k++) // complete the buffer with 0
				char_array_3[k] = '\0';

			EncodeBuffer(char_array_3, char_array_4);
			for (int k = 0; k < tmp + 1 ; k++)
				result += base64_chars[char_array_4[k]];

			while (tmp++ < 3) // add some 'padding'
				result += '=';
		}

		return result;
	}

	Buffer<char> MyBase64::Decode(char const * src)
	{
		assert(src != nullptr);

		static const int CHUNK_SIZE = 1024 * 16;

		SparseWriteBuffer<> writer(CHUNK_SIZE);

		size_t i = 0;
		int tmp = 0;

		char c;

		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		while (true)
		{
			c = src[i];
			if (c == 0 || c == '=')
				break;
			if (!IsBase64(c))
				break;

			// replace incomming byte by its index and pack it
			char_array_4[tmp++] = strchr(base64_chars, c) - base64_chars;
			if (tmp == 4) 
			{
				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (int k = 0 ; k < 3 ; ++k)
					writer << char_array_3[k];
				tmp = 0;
			}
			++i;
		}




#if 0
		static inline bool is_base64(unsigned char c) {
			return (isalnum(c) || (c == '+') || (c == '/'));
		}

		std::string base64_decode(std::string const& encoded_string) {
			int in_len = encoded_string.size();
			int i = 0;
			int j = 0;
			int in_ = 0;
			unsigned char char_array_4[4], char_array_3[3];
			std::string ret;

			while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
				char_array_4[i++] = encoded_string[in_]; in_++;
				if (i ==4) {
					for (i = 0; i <4; i++)
						char_array_4[i] = base64_chars.find(char_array_4[i]);

					char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
					char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
					char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

					for (i = 0; (i < 3); i++)
						ret += char_array_3[i];
					i = 0;
				}
			}

			if (i) {
				for (j = i; j <4; j++)
					char_array_4[j] = 0;

				for (j = 0; j <4; j++)
					char_array_4[j] = base64_chars.find(char_array_4[j]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
			}

			return ret;
		}
#endif

		Buffer<char> result = SharedBufferPolicy<char>::NewBuffer(writer.GetWrittenSize());
		writer.CopyToBuffer(result.data, result.bufsize);
		return result;
	}

}; // namespace chaos
