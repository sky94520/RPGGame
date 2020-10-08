# RPG��Ϸ����

>ּ�ڿ���2D�غ�����Ϸ��
>����Ŀ������[SDL_Engine](https://github.com/sky94520/SDL_Engine)��lua5.4

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
>MapLayer�е�һ��TiledMap�����ڵ�collision layerΪ����Ŀ����Ҫ�㣬
>�󲿷־��鶼��������ײ�㡣
## ������
>1. PlayerManager  
>���ǲ���� ֻ�Ǹ���������ɫ�������MapLayer����ײ����
>��Ҫ������������Ҫ��ײ��Ĳ��룺
>>1. initializePlayers() �Ӵ浵�ж�ȡ���ݲ�������Ҷ���ʱ����Ҫ�Ѷ�����ӵ��������
>>2. changeLayerOfPlayers() �ı�������ڵĲ� �����Ҫ��GameScene::changeMap�ڵ������ı�������
>
>2. ScriptManager  
> �������ű�������ű��������MapLayer����ײ����
> ��addLuaObject����Ҫ��ײ��
>
>5. SpritePool
>����أ����𴴽�����ͻ��վ������
>6. Text
>��������ʾ�Ի�
## �ű����� Good[�ö�����c++��Good���lua��Good�����]
>c++��Good���ṩ��һ���ӿڣ������Ĳ�������lua�ű���  
����Ϊ��������
>>1. Good:execute(userId, targetId)
>>- ��Ʒ��ִ�к���
>>- @param userId ʹ���ߵ�id
>>- @param targetId Ŀ��id
>>2. Good:clean()
>>- ����������c++��Good�����ڵ�������������ʱ�����øú���
>>3. Good:setIconID(iconId) ͼ��id
>>4. Good:setName(name) ������Ʒ����
>>5. Good:setDescription(desc) ��������
>>6. Good:setDeleption(ret) �����Ƿ�������Ʒ
>>7. Good:setNumber(num) ���ñ���Ʒ����
>>8. Good:setCost(cost) ���ü۸񣨲��������ܣ�
>>9. Good:setGoodType(type) ������Ʒ����(Item��Equipment��Skill��Seed)
>>10. Good:setUsageRange(ur) ����ʹ�÷�Χ(Self��SingleAlly��TotalAlly��SingleEnemy��TotalEnemy)
>  
>����Ϊװ�����к���
>>1. equip
>>2. unequip
>>3. �ȵ�
## �ű�
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
>>1. object.create(name, chartlet, tileX, tileY)
>>�����ű�����
>>- @param name �ű���������� Ψһ������ͨ�����name�������ýű�����
>>- @param chartlet ��������
>>- @param tileX ���ڵ�tileXλ��
>>- @param tileY ���ڵ�tileYλ��
>>- @return �Ƿ񴴽��ɹ�
>>
>>2. object.delete(name)
>>- @param name Ҫɾ���Ľű���������
>>
>>3. object.set_trigger(name, type)
>>���ýű�����Ĵ�����ʽ���Լ�������Ļص�����
>>- @param name �ű���������
>>- @param type �������� int=>TriggerType
>>- @param funcName lua������
>>
>>4. object.set_priority(name, type)
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

## ������
>1. character.xml => json��ʽ
>XML��ʽ�������ڵ�ǰ��Ӧ�ó�����Ŀǰ�ƻ�ʹ��cJsonObject����SDL_Engine ValueMap��ʹ��
>��Ҫ����������У�
>>1. ��������б���
>>2. key��dict���Ƿ����
>2. �ű�����Ķ�����ʾ����Ȼ����v1�汾��
>~~3. �ű��������ҵ��ڵ�����~~
>�������Ѿ����������Ŀǰ��Ҫ������ײ������⡣��ײ���Ǵ�����MapLayer::TiledMap�µ�һ��TMXLayer��
>Ŀǰ�����о��鶼�������������ģ�EffectLayer��PlayerManager��ScriptManager��GameScene
>4. ��Ҵ������ű�ʱ����ͣ���� �ڴ����ű�ʱ�������������ߣ�������ͣ��Ҳ�����ڽű���ֹͣ�˶�
