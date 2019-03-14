/*	Havok Format Library
	Copyright(C) 2016-2019 Lukas Cone

	This program is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.If not, see <https://www.gnu.org/licenses/>.
*/

#include "hkaSkeleton.h"
#include "hkRootLevelContainer.h"
#include "hkaAnimationContainer.h"
#include "datas/macroLoop.hpp"
#include "datas/jenkinshash.hpp"
#include "hkaDeltaCompressedAnimation.h"
#include "hkaWaveletCompressedAnimation.h"

#define hkRegisterClass(classname) { classname::HASH, &hkCreateDerivedClass<classname> },
#define hkRegisterClassID(classname)const JenHash classname::HASH = JenkinsHash(#classname, sizeof(#classname) - 1);

#define hkRegisterVersionedClassID(classname, version)\
hkRegisterClassID(classname##_t<##classname##version##_t<hkRealPointer>>);\
hkRegisterClassID(classname##_t<##classname##version##_t<hkFakePointer>>);\
hkRegisterClassID(classname##_t<##classname##version##_t<hkStripPointer>>);\
hkRegisterClassID(classname##_t<##classname##version##_rp_t<hkRealPointer>>);\
hkRegisterClassID(classname##_t<##classname##version##_rp_t<hkFakePointer>>);\
hkRegisterClassID(classname##_t<##classname##version##_rp_t<hkStripPointer>>);\

#define hkRegisterVersionedClass(classname, version)\
hkRegisterClass(classname##_t<##classname##version##_t<hkRealPointer>>)\
hkRegisterClass(classname##_t<##classname##version##_t<hkFakePointer>>)\
hkRegisterClass(classname##_t<##classname##version##_t<hkStripPointer>>)\
hkRegisterClass(classname##_t<##classname##version##_rp_t<hkRealPointer>>)\
hkRegisterClass(classname##_t<##classname##version##_rp_t<hkFakePointer>>)\
hkRegisterClass(classname##_t<##classname##version##_rp_t<hkStripPointer>>)\

#define hkRegisterVersionedClassEval(classname, id, version) hkRegisterVersionedClass(classname, version)

#define hkRegisterFullClassID(classname)\
hkRegisterVersionedClassID(classname, 550)\
hkRegisterVersionedClassID(classname, 660)\
hkRegisterVersionedClassID(classname, 710)\
hkRegisterVersionedClassID(classname, 2010)\
hkRegisterVersionedClassID(classname, 2011)\
hkRegisterVersionedClassID(classname, 2012)\
hkRegisterVersionedClassID(classname, 2013)\
hkRegisterVersionedClassID(classname, 2014)\
hkRegisterVersionedClassID(classname, 2016)

#define hkRegisterFullClass(classname)\
hkRegisterVersionedClass(classname, 550)\
hkRegisterVersionedClass(classname, 660)\
hkRegisterVersionedClass(classname, 710)\
hkRegisterVersionedClass(classname, 2010)\
hkRegisterVersionedClass(classname, 2011)\
hkRegisterVersionedClass(classname, 2012)\
hkRegisterVersionedClass(classname, 2013)\
hkRegisterVersionedClass(classname, 2014)\
hkRegisterVersionedClass(classname, 2016)

hkRegisterFullClassID(hkaSkeleton)
hkRegisterFullClassID(hkRootLevelContainer)
hkRegisterFullClassID(hkaAnimationContainer)

hkRegisterVersionedClassID(hkaDeltaCompressedSkeletalAnimation, 550)
hkRegisterVersionedClassID(hkaDeltaCompressedAnimation, 660)
hkRegisterVersionedClassID(hkaDeltaCompressedAnimation, 710)
hkRegisterVersionedClassID(hkaDeltaCompressedAnimation, 2010)

hkRegisterVersionedClassID(hkaWaveletCompressedSkeletalAnimation, 550)
hkRegisterVersionedClassID(hkaWaveletCompressedAnimation, 660)
hkRegisterVersionedClassID(hkaWaveletCompressedAnimation, 710)
hkRegisterVersionedClassID(hkaWaveletCompressedAnimation, 2010)

template<class C> hkVirtualClass *hkCreateDerivedClass() { return new C{}; }

static const std::map<JenHash, hkVirtualClass *(*)()> hkClassStorage =
{
	StaticFor(hkRegisterFullClass, hkaSkeleton, hkRootLevelContainer, hkaAnimationContainer)

	hkRegisterVersionedClass(hkaDeltaCompressedSkeletalAnimation, 550)
	StaticForArgID(hkRegisterVersionedClassEval, hkaDeltaCompressedAnimation, 660, 710, 2010)

	hkRegisterVersionedClass(hkaWaveletCompressedSkeletalAnimation, 550)
	StaticForArgID(hkRegisterVersionedClassEval, hkaWaveletCompressedAnimation, 660, 710, 2010)
};

hkVirtualClass *IhkPackFile::ConstructClass(JenHash hash)
{
	if (hkClassStorage.count(hash))
		return hkClassStorage.at(hash)();

	return nullptr;
}