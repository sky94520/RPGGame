#ifndef __GameMacros_H__
#define __GameMacros_H__

/*����,����ͼ�й�*/
enum class Direction
{
	Down = 0,
	Left,
	Right,
	Up,
};
/*���ȼ� */
#define PRIORITY_SAME 0 /*�������ܲ�����ײ*/
#define PRIORITY_LOW  1 /*NPC��ʾ����*/
#define PRIORITY_HIGH 2 /*NPC��ʾ����*/

#endif