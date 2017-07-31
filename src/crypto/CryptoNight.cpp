/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "crypto/CryptoNight.h"
#include "crypto/CryptoNight_p.h"
#include "crypto/CryptoNight_test.h"
#include "net/Job.h"
#include "net/JobResult.h"
#include "Options.h"
#include <iostream>

static inline char hf_bin2hex(unsigned char c){if(c<=0x9){return '0'+c;}return 'a'-0xA+c;}
void toHexx(const unsigned char* in,unsigned int len,char* out){for(unsigned int i=0;i<len;i++){out[i*2]=hf_bin2hex((in[i]&0xF0)>>4);out[i*2+1]=hf_bin2hex(in[i]&0x0F);}}

bool CryptoNight::hash(const Job &job, JobResult &result, cryptonight_ctx *ctx) {
	//cryptonight_hash<0x80000, MEMORY, 0x1FFFF0, false>(job.blob(), job.size(), result.result, ctx);
	CryptoNight::hashx(job.blob(), job.size(), result.result, ctx);
    return *reinterpret_cast<uint64_t*>(result.result + 24) < job.target();
}
void CryptoNight::hashx(const uint8_t *input, size_t size, uint8_t *output, cryptonight_ctx *ctx) {
	//std::cout << "input=" << std::endl << ctx << std::endl;
	//std::cout << ctx->state0 << std::endl; //"...........................";
	//char *out;
	//toHexx(input, 37, out);
	//std::cout << "out=" << out << std::endl;
	//std::cout << "input=" << std::endl << input << std::endl << "size=" << std::endl << size << std::endl << "output=" << std::endl << output << std::endl << "ctx=" << std::endl << ctx << std::endl;
	cryptonight_hash<0x80000, MEMORY, 0x1FFFF0, false>(input, size, output, ctx);
}
