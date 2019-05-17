#ifndef __GameMacros_H__
#define __GameMacros_H__

/*方向,跟贴图有关*/
enum class Direction
{
	Down = 0,
	Left,
	Right,
	Up,
};
enum class State
{
	None,
	Idle,
	Walking,
};
/*优先级 */
#define PRIORITY_SAME 0 /*与人物能产生碰撞*/
#define PRIORITY_LOW  1 /*NPC显示在下*/
#define PRIORITY_HIGH 2 /*NPC显示在上*/

#endif