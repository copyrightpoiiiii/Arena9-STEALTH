//
// A simple sample AI.
//

#include "sdk/sdk.h"
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstring>
#include <string>


#define eps 1e-4
#define pi 3.1415926536
struct bob {
    float x, y, vx, vy;
    int lun;
} a[40000];
struct att {
    Vec2 pos;
    int roun;
} c[40000];
Vec2 b[40000];
Vec2 tar;
Vec2 last_pos;
int tot = 0, tot2 = 0, tot3 = 0,tot4=0;
int hp_last = 100;
int num_mon;
int step = 10;
int duo;
int bombcnt;
int recbomb[40];
int nig[25] = {80, 190, 300, 410, 520, 630, 740, 850, 960, 1070, 2000};

float dis(Vec2 a, Vec2 b) {
    return SDK::distanceTo(a, b);
}

float dis2(Vec2 a, Vec2 b) {
    float xx = a.y - b.y, yy = a.x - b.x;
    return sqrt(xx * xx + yy * yy);
}

float fabs(float a) {
    return a < 0 ? -a : a;
}

bool isnight(int x) {
    if (x <= 0)return 1;
    x %= 110;
    if (x > 80)
        return 1;
    else return 0;
}

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')


AI_API void playerAI(const PlayerSight sight, Actions *actions) {
    if (fabs(tar.x - sight.pos.x) <= eps && fabs(tar.y - sight.pos.y) <= eps && (!duo))
        tar.x = -1;
    duo = 0;
    srand(time(NULL));
    bombcnt = sight.bombCount;
    for (int i = 0; i < tot; i++) {
        a[i].lun++;
    }
    tot4=0;
    memset(recbomb,0, sizeof(recbomb));
    /*
     * ????
     */
    num_mon = sight.gold;
    if (!sight.wardCount && sight.bombCount) {
        if (num_mon >= 5) {
            actions->emplace(BuyItem, WardItem);
            num_mon -= 5;
        }
    }
    if (!sight.bombCount) {
        if (num_mon >= 20) {
            actions->emplace(BuyItem, BombItem);
            num_mon -= 20;
            bombcnt++;
        }
    }
    if (sight.wardCount < 3 && sight.bombCount) {
        actions->emplace(BuyItem, WardItem);
        num_mon -= 5;
    }
    if (num_mon >= 20 && sight.bombCount <= 3) {
        actions->emplace(BuyItem, BombItem);
        num_mon -= 20;
        bombcnt++;
    }
    if (sight.hp < hp_last && (hp_last - sight.hp == 50 || hp_last - sight.hp == 55)) {
        if (sight.canUseBomb && bombcnt) {
            actions->emplace(UseItem, BombItem, last_pos);
        }
        int maxdis = 0;
        Vec2 tm;
        for (int i = 1; i <= 10; i++) {
            tm.x = rand() % 60 + 1;
            tm.y = rand() % 90 + 1;
            while (!SDK::reachable(tm)) {
                tm.x = rand() % 60 + 1;
                tm.y = rand() % 90 + 1;
            }
            if (dis(sight.pos, tm) > 20.0) {
                tar = tm;
                duo = 1;
                break;
            }
        }
    }
    /*
     * ??
     */
    if (sight.canUseWard && (sight.round - 60) % 110 == 0) {
        actions->emplace(UseItem, WardItem, sight.pos);
    } else {
        int j = 0;
        while (nig[j] < sight.round)
            j++;
        if (abs(nig[j] - sight.round) <= 10 || abs(nig[j - 1] - sight.round) <= 10) {
            if (sight.canUseWard)
                actions->emplace(UseItem, WardItem, sight.pos);
        }
    }
    /*
     * ????
     * ??????????????,????????
     * ??????????
     * ????????????????,??????????
     */

    if (sight.bombInSightCount) {
        int zhatar = 0;
        if (!SDK::reachable(tar))
            tar = sight.pos;
        for (int j = 0; j < tot; j++)
            if (a[j].lun <= 3) {
                Vec2 tmpos;
                tmpos.x = a[j].x + a[j].vx * 3;
                tmpos.y = a[j].y + a[j].vy * 3;
                if (dis2(tar, tmpos) <= 10.5)zhatar = 1;
            }
        for (int i = 0; i < sight.bombInSight.size(); i++) {
            int flag = 0;
            for (int j = 0; j < tot; j++) {
                if (a[j].lun <= 3 && a[j].x + a[j].vx * (a[j].lun - 1) == sight.bombInSight[i].pos.x) {
                    if (a[j].y + a[j].vy * (a[j].lun - 1) == sight.bombInSight[i].pos.y) {
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag)continue;
            bob tmp;
            tmp.lun = 1;
            tmp.vx = sight.bombInSight[i].velocity.x;
            tmp.vy = sight.bombInSight[i].velocity.y;
            tmp.x = sight.bombInSight[i].pos.x;
            tmp.y = sight.bombInSight[i].pos.y;
            if (tmp.x == last_pos.x && tmp.y == last_pos.y)
                continue;
            c[++tot3].pos.x = tmp.x+tmp.vx*3;
            c[tot3].pos.y = tmp.y+tmp.vy*3;
            c[tot3].roun = sight.round+3;
            a[tot++] = tmp;
            if (dis2(tar, sight.bombInSight[i].pos) <= 10.5)zhatar = 1;
            if (((!isnight(sight.round - 4))) ) {
                recbomb[++tot4]=i;
            }
        }
        if(tot4){
            int jiid=0,mxnum=0;
            for(int i=1;i<=tot4;i++){
                int nu=0;
                for(int j=1;j<=tot4;j++)
                    if(dis2(sight.bombInSight[recbomb[i]].pos,sight.bombInSight[recbomb[j]].pos)<=5.0)
                        nu++;
                if(nu>mxnum){
                    jiid=i;
                    mxnum=nu;
                }
            }
            actions->emplace(UseItem,BombItem,sight.bombInSight[recbomb[jiid]].pos);
            c[++tot3].pos.x = sight.bombInSight[recbomb[jiid]].pos.x;
            c[tot3].pos.y = sight.bombInSight[recbomb[jiid]].pos.y;
            c[tot3].roun = sight.round+3;
        }
        /*
        * ?????
        * ???????6????200??
        * ???????????????
        */
        if (zhatar == 1) {
            float r, theta;
            float di = 9999;
            for (int i = 1; i <= 70; i++) {
                r = rand() % 3 + 1;
                theta = rand() % 360 + 1;
                Vec2 tmp;
                int flag2 = 0;
                tmp.x = sight.pos.x + (float) r * sin((float) theta * pi / 180.0);
                tmp.y = sight.pos.y + (float) r * cos((float) theta * pi / 180.0);
                if (!SDK::reachable(tmp))
                    continue;
                for (int j = 0; j < tot; j++) {
                    if (a[j].lun <= 3) {
                        Vec2 tmp2;
                        tmp2.x = a[j].x + a[j].vx * 3;
                        tmp2.y = a[j].y + a[j].vy * 3;
                        if (dis2(tmp2, tmp) <= 11.0) {
                            flag2 = 1;
                            break;
                        }
                    }
                }
                if (!flag2) {
                    if (di > dis(sight.pos, tmp)) {
                        di = dis(sight.pos, tmp);
                        tar = tmp;
                    }
                }
            }
            actions->emplace(SelectDestination, tar);
        } else actions->emplace(SelectDestination, tar);
        duo = 1;
    }


    /*
     * ????
     * ??????????,????????
     */


    if (sight.corpseInSightCount) {
        for (int i = 0; i < sight.corpseInSight.size(); i++) {
            int flagco = 0;
            for (int j = 1; j <= tot2; j++)
                if (b[j].x == sight.corpseInSight[i].x && b[j].y == sight.corpseInSight[i].y) {
                    flagco = 1;
                    break;
                }
            if (!flagco) {
                b[++tot2].x = sight.corpseInSight[i].x;
                b[tot2].y = sight.corpseInSight[i].y;
            }
            for (int j = 1; j <= tot3; j++)
                if (dis2(c[j].pos, sight.corpseInSight[i]) <= 10.0 && (sight.round - c[j].roun) <= 18) {
                    flagco = 1;
                    break;
                }
            if (flagco)continue;
            int gailv = rand() % 2;
            if (!isnight(sight.round - 4) && !isnight(sight.round) && hp_last != 0 && sight.round > 30 &&
                (gailv == 0))
                actions->emplace(UseItem, BombItem, sight.corpseInSight[i]);
            //break;
        }
    }


    /*
     *  ????
     *  ????????3?,A??,??
     *  ???????????,??????????x???
     *  ???????????
     *  ????
     */
    int cnt = 0, id = 0;
    /*for (int i = 0; i < sight.unitInSight.size(); i++)
        if (dis2(sight.pos, sight.unitInSight[i].pos) <= 4.0) {
            cnt++;
            if (i != sight.id)
                id = sight.unitInSight[i].id;
        }
    if (cnt >= 3) {*/
    /*if (sight.canUseBomb) {
        actions->emplace(UseItem, BombItem, sight.pos);
    }*/
    /*if (sight.canSuckAttack) {
        actions->emplace(SuckAttack, id);
    }
}*/
    static StealthStrategy ss;
    if (sight.hp <= 90) {
        //ss.Disable();
        Vec2 tmptar;
        tmptar.x = -1;
        for (int i = 0; i < sight.unitInSight.size(); i++)
            if (dis2(sight.pos, sight.unitInSight[i].pos) <= 4.0 && sight.unitInSight[i].id != sight.id) {
                actions->emplace(SuckAttack, sight.unitInSight[i].id);
                tmptar = sight.unitInSight[i].pos;
                c[++tot3].roun = sight.round;
                c[tot3].pos = sight.unitInSight[i].pos;
                break;
            }
        if (!duo) {
            if (SDK::reachable(tmptar)) {
                int r = 10, theta;
                float dista = 999;
                for (int i = 1; i <= 50; i++) {
                    theta = rand() % 360 + 1;
                    Vec2 taoa;
                    taoa.x = sight.pos.x + (float) r * sin((float) theta * pi / 180.0);
                    taoa.y = sight.pos.y + (float) r * cos((float) theta * pi / 180.0);
                    if (SDK::reachable(taoa)) {
                        if (dis(taoa, sight.pos) < dista) {
                            tar = taoa;
                            dista = dis(taoa, sight.pos);
                        }
                    }
                }
                actions->emplace(SelectDestination, tar);
            } else if (SDK::reachable(tar) && tar.x != sight.pos.x && tar.x != sight.pos.y) {
                actions->emplace(SelectDestination, tar);
            } else {
                ss.Enable();
                ss.generateActions(sight, actions);
                ss.Disable();
            }
        }

    } else {
        if (!duo) {
            ss.Enable();
            ss.generateActions(sight, actions);
            ss.Disable();
        }
    }
    //else ss.Disable();
    /*cnt = 0;
    if (sight.hp <= 13) {
        int id;
        for (int i = 0; i < sight.unitInSight.size(); i++) {
            int cnt1 = 0;
            for (int j = 0; j < sight.unitInSight.size(); j++) {
                if (i == j)continue;
                if (dis2(sight.unitInSight[i].pos, sight.unitInSight[j].pos) <= 5.0)
                    cnt1++;
            }
            if (cnt1 >= cnt) {
                cnt = cnt1;
                id = i;
            }
        }
        if (sight.canUseBomb)
            actions->emplace(UseItem, BombItem, sight.unitInSight[id].pos);
    }*/

    /*
     * ???????
     */
    /*
    int x = 0, y = 0, jx, jy;
    cnt = 9999;
    float di = 9999;
    for (int i = 1; i <= 200; i++) {
        x = rand() % 60;
        y = rand() % 90;
        int cnt1 = 0;
        Vec2 tmp;
        tmp.x = x;
        tmp.y = y;
        for (int k = 0; k < sight.unitInSight.size(); k++)
            if (dis2(tmp, sight.unitInSight[k].pos) <= 4.0)
                cnt1++;
        if (cnt1 < cnt) {
            jx = x;
            jy = y;
            cnt = cnt1;
            di = dis(tmp, sight.pos);
        } else if (cnt1 == cnt) {
            if (di > dis(tmp, sight.pos)) {
                di = dis(tmp, sight.pos);
                jx = x;
                jy = y;
            }
        }
    }
    Vec2 des;
    des.x = jx;
    des.y = jy;
    step++;
    if (step > 10) {
        actions->emplace(SelectDestination, des);
        step = 0;
    }
    */

    /*
     * ???????
     */


    /*
     *  ????
     *  ???????????????
     *  ??A????
     */
    last_pos = sight.pos;
    hp_last = sight.hp;
}

}

