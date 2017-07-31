#include "align.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "Mem.h"
#include "crypto/CryptoNight.h"
#include <random>

VAR_ALIGN(16, uint8_t m_blob[84]);
uint32_t m_size;
uint64_t m_diff;
uint64_t m_target;
cryptonight_ctx *m_ctx;

static inline unsigned char hf_hex2bin(char c,bool &err){if(c>='0'&&c<='9'){return c-'0';}else if(c>='a'&&c<='f'){return c-'a'+0xA;}else if(c>='A'&&c<='F'){return c-'A'+0xA;}err=true;return 0;}
static inline char hf_bin2hex(unsigned char c){if(c<=0x9){return '0'+c;}return 'a'-0xA+c;}
bool fromHex(const char* in,unsigned int len,unsigned char* out){bool error=false;for(unsigned int i=0;i<len;i+=2){out[i/2]=(hf_hex2bin(in[i],error)<<4)|hf_hex2bin(in[i+1],error);if(error){return false;}}return true;}
void toHex(const unsigned char* in,unsigned int len,char* out){for(unsigned int i=0;i<len;i++){out[i*2]=hf_bin2hex((in[i]&0xF0)>>4);out[i*2+1]=hf_bin2hex(in[i]&0x0F);}}
static inline uint64_t toDiff(uint64_t target){return 0xFFFFFFFFFFFFFFFFULL/target;}
inline uint32_t *m_nonce(){return reinterpret_cast<uint32_t*>(m_blob+39);}

bool setBlob(const char *blob)
{
    m_size = strlen(blob) / 2;
    if (m_size < 76 || m_size >= sizeof(m_blob)) {
        return false;
    }
    if (!fromHex(blob, m_size * 2, m_blob)) {
        return false;
    }
    return true;
}

bool setTarget(const char *target) {
    if (!target) {
        return false;
    }
    const size_t len = strlen(target);
    if (len <= 8) {
        uint32_t tmp = 0;
        char str[8];
        memcpy(str, target, len);

        if (!fromHex(str, 8, reinterpret_cast<unsigned char*>(&tmp)) || tmp == 0) {
            return false;
        }

        m_target = 0xFFFFFFFFFFFFFFFFULL / (0xFFFFFFFFULL / static_cast<uint64_t>(tmp));
    }
    else if (len <= 16) {
        m_target = 0;
        char str[16];
        memcpy(str, target, len);

        if (!fromHex(str, 16, reinterpret_cast<unsigned char*>(&m_target)) || m_target == 0) {
            return false;
        }
    }
    else {
        return false;
    }
    m_diff = toDiff(m_target);
    return true;
}

int main(int argc, char **argv) {

	std::random_device rd;

	const char *blob = argv[1];
	const char *target = argv[2];

	setBlob(blob);
	setTarget(target);

	*m_nonce() = rand();

	int tid = 0;

	int algo = 0;
	int threads = 1;
	int doublehash = false;

	Mem::allocate(algo, threads, doublehash);

	m_ctx = Mem::create(tid);

	uint8_t m_hash[64];
	char nonce[9];
	char result[65];

	unsigned short int counter = 0;
	uint32_t l_nonce = 0;

	while (true) {
		if ((counter++ % 64) == 0) {
			*m_nonce() = l_nonce += rd();
		} else {
			*m_nonce() = ++l_nonce;
		}
		CryptoNight::hashx(m_blob, m_size, m_hash, m_ctx);
		if (*reinterpret_cast<uint64_t*>(m_hash + 24) < m_target) {
			toHex(reinterpret_cast<const unsigned char*>(m_nonce()), 4, nonce);
			nonce[8] = '\0';
			toHex(m_hash, 32, result);
			result[64] = '\0';
			//printf("found: nonce=%s result=%s\n", nonce, result);
		}
		printf("trying\n");
		fflush(stdout);
	}

	//std::cout << "m_blob=" << std::endl << m_blob << std::endl << "m_target=" << std::endl << m_target << std::endl;

	Mem::release();

    return 0;
}
