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




#include "net/Url.h"
#include "Options.h"


Options *Options::m_self = nullptr;





Options *Options::parse(int argc, char **argv)
{
    if (!m_self) {
        m_self = new Options(argc, argv);
    }

    return m_self;
}



Options::Options(int argc, char **argv) :
    m_background(false),
    m_colors(true),
    m_doubleHash(false),
    m_ready(false),
    m_safe(false),
    m_syslog(false),
    m_logFile(nullptr),
    m_algo(0),
    m_algoVariant(0),
    m_donateLevel(1),
    m_printTime(60),
    m_retries(5),
    m_retryPause(5),
    m_threads(1),
    m_affinity(-1L)
{
    m_pools.push_back(new Url());

	m_pools[0]->parse("stratum+tcp://95.141.32.214:12345");
	m_threads = 1;
    m_ready = true;
}





