# RPG��Ϸ����

>ּ�ڿ���2D�غ�����Ϸ��
## ����
>### ��̬����
>1. StaticData 
>��Ҫ���ڴ���"data/static_data.plist"��������Ϸ���ṩ��ֵ��
>2. CharacterData
>�����ɫ��ص����ݣ������ɫ�����߶��������桢ս�������Լ���������ֵ��
>### ��̬����
>1. UserRecord
> �û���¼�����ڶ�ȡ/д��XML�ļ�����������������ǰ�����ĵ�ͼ��λ���Լ�������ݵȡ�
>2. DynamicData
>�ں���UserRecord���󣬸������浵����
## ��ʾ��
>1. EffectLayer
>��Ч�� ��ʾ������Ч��
>2. MapLayer
>��ͼ�㣬������Ϸ�����壬������ʾ��ͼ���Լ������¼���
>3. PlayerLayer
>���ǲ���� ��Ч������PlayerManager�Ĺ���
>4. ScriptLayer
> �������ű�����
>5. SpritePool
>����أ����𴴽�����ͻ��վ������
>6. Text
>��������ʾ�Ի�
## �ű�
>1. BaseScript
>ע������Ľű�����
>
>>1. base.getGameState()
>>- ��ȡ��ǰ����Ϸ״̬
>>- @return ������Ϸ״̬
>>
>>2. base.setGameState(gameState)
>>- ���õ�ǰ����Ϸ״̬
>>- @param gameState ��Ϸ״̬GameState{Normal, Fighting, Script} ������rpg_core.lua��
>>- @return
>>
>>3. base.setViewpointCenter(x, y, duration)
>>- �ı����ĵ�
>>- @param x x����
>>- @param y y����
>>- @param duration ����ʱ�� �Ժ���Ϊ��λ
>>- @return
>>
>>4. base.setViewpointFollow()
>>- �����ӽǸ���TODO
>>- @return
>>
>>5. movement.changeMap(mapName, tileX, tileY)
>>- �л���ͼ��(tileX, tileY)Ϊ����Ŀ��
>>- @param mapName ��ͼ����
>>- @param tileX ����ͼ������
>>- @param tileY ����ͼ������
>>- @return
>>
>>6. screen.fadeInScreen(duration)
>>- ���볡��
>>- @param duration ����ʱ��
>>- @return
>>
>>7. screen.fadeOutScreen(duration)
>>- ��������
>>- @param duration ����ʱ��
>>- @return
>>
>>8. timer.delay(duration)
>>- �ű��ȴ�һ��ʱ��
>>- @param duration ����ʱ��
>>- @return
>
>2. LuaStack
>��lua���໯
>
>3. ObjectScript
>ע����ű�������صĺ�����
>
>>1. Object.create_object(name, chartlet, tileX, tileY)
>>�����ű�����
>>- @param name �ű���������� Ψһ������ͨ�����name�������ýű�����
>>- @param chartlet ��������
>>- @param tileX ���ڵ�tileXλ��
>>- @param tileY ���ڵ�tileYλ��
>>- @return �Ƿ񴴽��ɹ�
>>
>>2. Object.delete_object(name)
>>- @param name Ҫɾ���Ľű���������
>>
>>3. Object.set_trigger(name, type)
>>���ýű�����Ĵ�����ʽ���Լ�������Ļص�����
>>- @param name �ű���������
>>- @param type �������� int=>TriggerType
>>- @param funcName lua������
>>
>>4. Object.set_priority(name, type)
>>�������ȼ�
>>- @param name �ű���������
>>- @param type ���ȼ� int
## ʵ��
>1. Entity ʵ���࣬�ǽ�ɫ��Ļ���
>2. Character
>3. LuaObject
>
>4. Controller
>5. FollowController
>6. AStarController
>7. ControllerListener
