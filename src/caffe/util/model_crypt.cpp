
#include "caffe/cc/core/cc.h"
#include "caffe/util/model_crypt.h"
#include "caffe/util/pa_file.h"
#include <Windows.h>

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

static unsigned char g_password_part[] = { 0x1c, 0x23, 0x53, 0xA1, 0x55, 0xc3, 0x01, 0x00, 0xF0, 0x3F, 0x31, 0x11, 0xAE, 0x35, 0x85, 0x16 };
static unsigned char g_password_package[] = { 0xcc, 0x15, 0x31, 0xA1, 0xec, 0x2c, 0x56, 0x82, 0x81, 0x39, 0x71, 0x81, 0xe1, 0x35, 0x85, 0x1c };

static uchar auchCRCHi[] =
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

static uchar auchCRCLo[] =
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

static void rc4_crypt(const unsigned char* password, int lenpasswod, unsigned char* data, unsigned long len)
{
	unsigned char sbox[256];
	int rand_index[] = { 13, 29, 51, 71, 97, 121, 153 };
	int rand_len = sizeof(rand_index) / sizeof(rand_index[0]);
	for (int i = 0; i < 256; ++i)
		sbox[i] = i ^ ((rand_index[i % rand_len] ^ 0x0AA0CF01) % len);

	{
		int i = 0, j = 0;
		unsigned char k[256] = { 0 };
		unsigned char tmp = 0;
		for (i = 0; i < 256; i++)
			k[i] = (unsigned char)password[i % lenpasswod];

		for (i = 0; i < 256; i++) {
			j = (j + sbox[i] + k[i] + rand_index[sbox[i] % rand_len]) % 256;
			tmp = sbox[i];
			sbox[i] = sbox[j];//����s[i]��s[j]
			sbox[j] = tmp;
		}
	}

	{
		int i = 0, j = 0, t = 0;
		unsigned long k = 0;
		unsigned char tmp;
		for (k = 0; k < len; k++)
		{
			i = (i + 1) % 256;
			j = (j + sbox[i]) % 256;
			tmp = sbox[i];
			sbox[i] = sbox[j];//����s[x]��s[y]
			sbox[j] = tmp;
			t = (sbox[i] + sbox[j]) % 256;
			data[k] ^= sbox[t];
		}
	}
}

static unsigned short CRC16Body(uchar* msg, int len){
	uchar crchi = 0xFF;
	uchar crclo = 0xFF;
	unsigned int uindex;
	while (len--){
		uindex = crchi ^ *msg++;
		crchi = crclo ^ auchCRCHi[uindex];
		crclo = auchCRCLo[uindex];
	}
	return (crchi << 8 | crclo);
}

static void CRC16_package(uchar* msg, int len){
	unsigned short v = CRC16Body(msg, len - 2);
	msg[len - 2] = (uchar)(v >> 8);
	msg[len - 1] = (uchar)(v & 0xFF);
}

static bool checkCRC16(uchar* msg, int len){
	unsigned short v = CRC16Body(msg, len - 2);
	uchar tmp = msg[len - 2];
	msg[len - 2] = msg[len - 1];
	msg[len - 1] = tmp;
	unsigned short crc = *((unsigned short*)(msg + len - 2));
	msg[len - 1] = msg[len - 2];
	msg[len - 2] = tmp;
	return v == crc;
}

//////////////////////////////////////////////////////////////////
bool Package::open(const char* savefile){
	close();
	f = fopen(savefile, "wb");
	if (!f) return false;

	int hand[10] = { 0xAAC0FFE0, 0xE01FD25E, 0xECFFDDAE };
	fwrite(hand, 1, sizeof(hand), f);
	return true;
}

void Package::push(const void* data, int len, int type){
	if (!f) return;

	len = max(4 * sizeof(int), len);
	char* buffer = new char[len];
	memset(buffer, 0, len);
	CV_Assert(len >= 4 * sizeof(int));	//4 * sizeof(int)

	memcpy(buffer, data, len);
	rc4_crypt(g_password_part, sizeof(g_password_part), (unsigned char*)buffer, len);
	int v = *((int*)buffer + 3);
	int crc16 = CRC16Body((unsigned char*)buffer, len);

	int len_write = len ^ v;
	int type_write = type ^ v;
	int crc16_write = crc16 ^ v;
	int write_val[] = { len_write, type_write, crc16_write };
	fwrite(write_val, 1, sizeof(write_val), f);
	fwrite(buffer, 1, len, f);
	delete[] buffer;
}

void Package::close(){
	if (f) fclose(f);
}

Package::~Package(){
	close();
}
//////////////////////////////////////////////////////////////////////////

PackageDecode::PackageDecode(){
}

PackageDecode::~PackageDecode(){
	for (int i = 0; i < deploy_data.size(); ++i)
		delete[]deploy_data[i];

	deploy_data.clear();
	sizes.clear();
	types.clear();
}

bool PackageDecode::decode(const char* dat){
	FILE* f = fopen(dat, "rb");
	if (!f) return false;

	int hand[10] = { 0xAAC0FFE0, 0xE01FD25E, 0xECFFDDAE };
	fread(hand, 1, sizeof(hand), f);
	if (hand[0] != 0xAAC0FFE0){
		fclose(f);
		return false;
	}

	bool result = true;
	while (!feof(f)){
		int len_mask[7];
		int readn = fread(len_mask, 1, sizeof(len_mask), f);
		if (readn < sizeof(len_mask)) break;

		int len = len_mask[0] ^ len_mask[6];
		int type = len_mask[1] ^ len_mask[6];
		int crc16 = len_mask[2] ^ len_mask[6];
		int last_data = sizeof(len_mask)-sizeof(int)* 3;	//ȥ��len��type��crc16
		int read_len = len - last_data;
		uchar* data = new uchar[len];
		memcpy(data, &len_mask[3], last_data);
		fread(data + last_data, 1, read_len, f);

		int crc16_calc = CRC16Body((unsigned char*)data, len);
		if (crc16 != crc16_calc){
			result = false;
			break;
		}

		rc4_crypt(g_password_part, sizeof(g_password_part), (unsigned char*)data, len);
		sizes.push_back(len);
		deploy_data.push_back(data);
		types.push_back(type);
	}
	fclose(f);
	return result;
}

int PackageDecode::find(int type){
	for (int i = 0; i < types.size(); ++i){
		if (types[i] == type)
			return i;
	}
	return -1;
}

int PackageDecode::count(){
	return sizes.size();
}

size_t PackageDecode::size(int index){
	CV_Assert(index < sizes.size());
	return sizes[index];
}

uchar* PackageDecode::data(int index){
	CV_Assert(index < deploy_data.size());
	return deploy_data[index];
}

int PackageDecode::type(int index){
	CV_Assert(index < types.size());
	return types[index];
}

struct ModelInfo{
	char caffemodel[260];
	char deploy[260];
	char dir[260];
	char labels[260];
};

static bool package(const ModelInfo& model){
	const char* deploy = model.deploy;
	const char* caffemodel = model.caffemodel;
	const char* labels_file = model.labels;
	Package package;
	char save_path[260];
	bool result = false;

	sprintf(save_path, "%s/model.dat", model.dir);
	if (!package.open(save_path)) return false;

	size_t size_caffemodel;
	uchar* data_caffemodel = paReadFile(caffemodel, &size_caffemodel);
	if (!data_caffemodel) goto fail;

	size_t size_prototxt;
	uchar* data_prototxt = paReadFile(deploy, &size_prototxt);
	if (!data_prototxt) goto fail;

	size_t size_labels;
	uchar* data_labels = paReadFile(labels_file, &size_labels);
	if (!data_labels) goto fail;

	package.push(data_prototxt, size_prototxt, PART_TYPE_DEPLOY);
	package.push(data_caffemodel, size_caffemodel, PART_TYPE_CAFFEMODEL);
	package.push(data_labels, size_labels, PART_TYPE_LABELS);

	char names[3000] = {0};
	sprintf(names, "%s|%s|%s", deploy, caffemodel, labels_file);
	package.push(names, strlen(names)+1, PART_TYPE_NAMES);

	package.close();
	result = true;

fail:
	freeReadFile(&data_caffemodel);
	freeReadFile(&data_prototxt);
	freeReadFile(&data_labels);
	return result;
}

static string findOne(const string& dir, const char* filter){
	PaVfiles vfs;
	paFindFiles(dir.c_str(), vfs, filter, false, true);

	if (vfs.size() == 0) return "";
	return vfs[0];
}

static vector<ModelInfo> enumModels(const char* dir){

	vector<ModelInfo> ms;
	PaVfiles vds;
	paFindFiles(dir, vds, "*", false, true, PaFindFileType_Directory);

	for (int i = 0; i < vds.size(); ++i){
		string caffemodel = findOne(vds[i], "*.caffemodel");
		string deploy = findOne(vds[i], "*deploy*");
		string labels = findOne(vds[i], "*label*.txt");

		if (!caffemodel.empty() && !deploy.empty() && !labels.empty()){
			ModelInfo model;
			strcpy(model.caffemodel, caffemodel.c_str());
			strcpy(model.deploy, deploy.c_str());
			strcpy(model.dir, vds[i].c_str());
			strcpy(model.labels, labels.c_str());
			ms.push_back(model);
		}
	}
	return ms;
}

CCAPI bool CCCALL crypt_model(const char* dir){

	auto models = enumModels(dir);
	if (models.size() == 0) return false;

	bool ok = true;
	for (auto& model : models){
		if (!package(model)){
			//ERR("can't encode model.");
			ok = false;
		}
		else{
			DeleteFileA(model.caffemodel);
			DeleteFileA(model.deploy);
		}
	}
	return ok;
}