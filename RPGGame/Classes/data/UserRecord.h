#ifndef __UserRecord_H__
#define __UserRecord_H__

#include <string>

#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

class UserRecord : public Object
{
public:
	UserRecord();
	~UserRecord();
	CREATE_FUNC(UserRecord);

	//���ļ��ж�ȡ����
	bool readFromXML(const string& filename);
	//�������ݵ��ļ���
	bool writeToXML(const string& filename);
private:
	void parsePlayer(rapidxml::xml_node<>* root);
	void parseBag(rapidxml::xml_node<>* root);
private:
	//�������
	int m_goldNumber;
	//���۱���
	float m_sellRatio;
	//��ͼ
	string m_mapFilename;
	//���λ��
	Point m_tilePosOfPlayer;
};
#endif