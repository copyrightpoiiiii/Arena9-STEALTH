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
using namespace std;
struct bob {
    double x, y, vx, vy;
    int lun;
} a[40000];
struct att {
    Vec2 pos;
    int roun;
} c[40000];
Vec2 b[40000];
Vec2 tar = (Vec2) {-1, -1};
Vec2 last_pos;
int recbomb[50], recbomb1[50];
int tot = 0, tot2 = 0, tot3 = 0, tot4 = 0, tot5 = 0;
int hp_last = 100;
int head = 1000, tail = 0;
int num_mon;
int step = 10;
int duo;
int bombcnt;
int clue = 0;
int nig[25] = {80, 190, 300, 410, 520, 630, 740, 850, 960, 1070, 2000};
int jiluid = -1;

struct unitiS {
    int live, zha, id, din,lround;
    double index;
    Vec2 pos, vlo, last_pos;
} s[40000];
int book[40000];

double dis(Vec2 a, Vec2 b) {
    return SDK::distanceTo(a, b);
}

double dis2(Vec2 a, Vec2 b) {
    double xx = a.y - b.y, yy = a.x - b.x;
    return sqrt(xx * xx + yy * yy);
}

double fabs(double a) {
    return a < 0 ? -a : a;
}

bool isnight(int x) {
    if (x <= 0)return 1;
    x %= 110;
    if (x > 80)
        return 1;
    else return 0;
}

void init(const PlayerSight sight) {
    int tmptail = tail;
    if (sight.round == 1) {
        for (int i = 0; i < sight.unitInSight.size(); i++) {
            head = min(head, sight.unitInSight[i].id);
            tail = max(tail, sight.unitInSight[i].id);
            s[sight.unitInSight[i].id].id = sight.unitInSight[i].id;
            s[sight.unitInSight[i].id].pos = sight.unitInSight[i].pos;
            s[sight.unitInSight[i].id].vlo = sight.unitInSight[i].velocity;
            s[sight.unitInSight[i].id].index = 0;
            s[sight.unitInSight[i].id].live = 1;
            s[sight.unitInSight[i].id].zha = 0;
            s[sight.unitInSight[i].id].din = 0;
            s[sight.unitInSight[i].id].lround = sight.round;
        }
    } else {
        memset(book, 0, sizeof(book));
        for (int i = 0; i < sight.unitInSight.size(); i++)
            if (sight.unitInSight[i].id > tmptail) {
                s[sight.unitInSight[i].id].id = sight.unitInSight[i].id;
                tail = max(tail, sight.unitInSight[i].id);
                s[sight.unitInSight[i].id].zha = 0;
                s[sight.unitInSight[i].id].pos = sight.unitInSight[i].pos;
                s[sight.unitInSight[i].id].vlo = sight.unitInSight[i].velocity;
                s[sight.unitInSight[i].id].index = 0;
                s[sight.unitInSight[i].id].live = 1;
                book[sight.unitInSight[i].id] = 1;
                s[sight.unitInSight[i].id].din = 0;
                s[sight.unitInSight[i].id].lround = sight.round;
            } else {
                if (fabs(sight.unitInSight[i].pos.x - s[sight.unitInSight[i].id].pos.x) <= eps)
                    if (fabs(sight.unitInSight[i].pos.y - s[sight.unitInSight[i].id].pos.y) <= eps)
                        s[sight.unitInSight[i].id].din++;
                    else {
                        if(s[sight.unitInSight[i].id].din<10&&sight.round-s[sight.unitInSight[i].id].lround>5)
                            s[sight.unitInSight[i].id].index+=1.0;
                        s[sight.unitInSight[i].id].din = 1;
                    }
                s[sight.unitInSight[i].id].pos = sight.unitInSight[i].pos;
                s[sight.unitInSight[i].id].vlo = sight.unitInSight[i].velocity;
                s[sight.unitInSight[i].id].live = 1;
                book[sight.unitInSight[i].id] = 1;
                if (s[sight.unitInSight[i].id].din > 20)
                    s[sight.unitInSight[i].id].index += 1.0;
            }
        for (int i = head; i <= tail; i++)
            if (!book[i])
                s[i].live = 0;
        while (!s[head].live)
            head++;
    }
}

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')


AI_API void playerAI(const PlayerSight sight, Actions *actions) {
    freopen("output.txt","a+",stdin);
    //cout<<"the: "<<sight.round<<endl;
    init(sight);
    memset(recbomb, 0, sizeof(0));
    memset(recbomb, 0, sizeof(0));
    tot4 = 0;
    tot5 = 0;
    if (fabs(tar.x - sight.pos.x) <= eps && fabs(tar.y - sight.pos.y) <= eps) {
        tar.x = -1;
        duo = 0;
    }
    //cout<<duo<<" "<<sight.pos.x<<" "<<sight.pos.y<<" "<<tar.x<<" "<<tar.y<<endl;
    srand(time(NULL));
    bombcnt = sight.bombCount;
    for (int i = 0; i < tot; i++) {
        a[i].lun++;
    }
    //cout<<"1 lun"<<endl;
    /*
     * 购买道具
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
    if (sight.wardCount == 0 && sight.bombCount) {
        actions->emplace(BuyItem, WardItem);
        num_mon -= 5;
    }
    if (num_mon >= 20 ) {
        actions->emplace(BuyItem, BombItem);
        num_mon -= 20;
        bombcnt++;
    }
    /*
     * 插眼
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

    //cout<<"2 lun"<<endl;
    /*
     * 炸弹模式
     * 预判出所有新出现的炸弹的位置，向反方向进行躲避
     * 流星雨了解一下。。。
     * 如果有一个范围出现多个炸弹投掷者，向他们的中心投掷炸弹
     */

    if (sight.bombInSightCount > 0) {
        int zhatar = 0;
        if (!SDK::reachable(tar))
            tar = sight.pos;
        for (int j = 0; j < tot; j++)
            if (a[j].lun < 4) {
                Vec2 tmpos;
                tmpos.x = a[j].x + a[j].vx * 3;
                tmpos.y = a[j].y + a[j].vy * 3;
                if (dis2(tar, tmpos) <= 10.5)zhatar = 1;
            }
        for (int i = 0; i < sight.bombInSight.size(); i++) {
            //std:://cout<<a[i].vx<<" "<<a[i].vy<<std::endl;
            int flag = 0;
            for (int j = 0; j < tot; j++) {
                if (a[j].lun < 3 && fabs(a[j].x + a[j].vx * (a[j].lun - 1) - sight.bombInSight[i].pos.x) <= eps) {
                    if (fabs(a[j].y + a[j].vy * (a[j].lun - 1) - sight.bombInSight[i].pos.y) <= eps) {
                        flag = 1;
                        break;
                    }
                } else if (a[j].lun == 3 &&
                           fabs(a[j].x + a[j].vx * (a[j].lun - 1) - sight.bombInSight[i].pos.x) <= eps) {
                    if (fabs(a[j].y + a[j].vy * (a[j].lun - 1) - sight.bombInSight[i].pos.y) <= eps) {
                        flag = 1;
                        for (int k = head; k <= tail; k++)
                            if (s[k].live) {
                                if (fabs(dis2(s[k].pos, sight.bombInSight[i].pos) - 10.0) <= eps) {
                                    s[k].zha = 1;
                                }
                            }
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
            if (fabs(tmp.x - last_pos.x) <= eps && fabs(tmp.y - last_pos.y) <= eps)
                continue;
            c[++tot3].pos.x = tmp.x + tmp.vx * 3;
            c[tot3].pos.y = tmp.y + tmp.vy * 3;
            c[tot3].roun = sight.round + 3;
            a[tot++] = tmp;
            Vec2 finapos;
            finapos.x = tmp.x + tmp.vx * 3;
            finapos.y = tmp.y + tmp.vy * 3;
            if (dis2(tar, finapos) <= 10.5)zhatar = 1;
            if (dis2(sight.pos, finapos) <= 10.5)zhatar = 1;
            int cntdiu = 0;
            for (int j = 0; j < sight.unitInSight.size(); j++)
                if (fabs(dis2(sight.unitInSight[j].pos, sight.bombInSight[i].pos) - 2.0) <= eps)
                    cntdiu++;
            for (int j = 0; j < sight.unitInSight.size(); j++)
                if (fabs(dis2(sight.unitInSight[j].pos, sight.bombInSight[i].pos) - 2.0) <= eps)
                    s[sight.unitInSight[j].id].index += 1.0 / cntdiu;
        }
        //cout<<"3 lun"<<endl;
        /*
        * 走位躲炸弹
        * 随机选择半径为6的圆中的200个点
        * 选择不会被炸到并且最近的那个点
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
        }
        //if(dis(tar,sight.pos)>4.0)
        actions->emplace(SelectDestination, tar);
        //cout<<duo<<" "<<sight.pos.x<<" "<<sight.pos.y<<" "<<tar.x<<" "<<tar.y<<endl;
        duo = 1;
        //std:://cout<<"ssss"<<std::endl;
        /*for (int i = 0; i < tot; i++) {
            Vec2 tmp;
            tmp.x = a[i].x;
            tmp.y = a[i].y;
            //std:://cout<<dis2(tmp,tar)<<std::endl;
        }*/
        ////cout<<"1"<<endl;
        ////cout<<tar.x<<" "<<tar.y<<endl;
    }
    //cout<<"4 lun"<<endl;
    if (dis(sight.pos, tar) > 4.0 && SDK::reachable(tar)) {
        actions->emplace(SelectDestination, tar);
        //cout<<duo<<" "<<sight.pos.x<<" "<<sight.pos.y<<" "<<tar.x<<" "<<tar.y<<endl;
    }
    /*
     * 追杀模式
     * 追杀距离最近的那个没被炸死的人
     */
    if (jiluid == -1) {
        float minmdis = 999;
        for (int i = head; i <= tail; i++)
            if (s[i].id != sight.id) {
                if (s[i].zha && s[i].live) {
                    if (dis(sight.pos, s[i].pos) < minmdis) {
                        jiluid = s[i].id;
                        minmdis = dis(sight.pos, s[i].pos);
                    }
                }
            }
    }
    if (sight.canUseBomb && jiluid != -1) {
        int jiid = -1, maxnum = 0;
        for (int i = head; i < tail; i++)
            if (s[i].live && s[i].zha) {
                int nu = 0;
                for (int j = head; j < tail; j++)
                    if (dis2(s[i].pos, s[j].pos) <= 6.0&&s[j].zha)
                        nu++;
                if (nu > maxnum) {
                    jiid = i;
                    maxnum = nu;
                }
            }
        if (maxnum>=2) {
            actions->emplace(UseItem, BombItem, s[jiid].pos);
            a[tot].x = sight.pos.x + (s[jiid].pos.x - sight.pos.x) / 3;
            a[tot].y = sight.pos.y + (s[jiid].pos.y - sight.pos.y) / 3;
            a[tot].lun = 2;
            a[tot].vx = (s[jiid].pos.x - sight.pos.x) / 3;
            a[tot].vy = (s[jiid].pos.y - sight.pos.y) / 3;
            tot++;
            c[++tot3].pos = s[jiid].pos;
            c[tot3].roun = sight.round + 3;
        }
    }
    if (jiluid != -1) {
        if (s[jiluid].live) {
            actions->emplace(SelectDestination, s[jiluid].pos);
            tar = s[jiluid].pos;
            //cout<<duo<<" "<<sight.pos.x<<" "<<sight.pos.y<<" "<<tar.x<<" "<<tar.y<<endl;
            //duo = 1;
            if (sight.canSuckAttack && dis2(sight.pos, s[jiluid].pos) <= 4.0) {
                actions->emplace(SuckAttack, jiluid);
            }
            ////cout<<"2"<<endl;
            ////cout<<s[jiluid].live<<" "<<s[jiluid].pos.x<<" "<<s[jiluid].pos.y<<" "<<jiluid<<endl;
        } else {
            jiluid = -1;
        }
    }

    if (sight.hp < hp_last && (hp_last - sight.hp > 10 && hp_last - sight.hp < 80)) {
        int suckflag = 0;
        for (int i = 0; i < sight.unitInSight.size(); i++)
            if (dis2(last_pos, sight.unitInSight[i].pos) <= 6.0 && sight.id != sight.unitInSight[i].id) {
                suckflag = 1;
                break;
            }
        if (sight.canUseBomb && bombcnt && suckflag) {
            actions->emplace(UseItem, BombItem, last_pos);
            a[tot].x = sight.pos.x + (last_pos.x - sight.pos.x) / 3;
            a[tot].y = sight.pos.y + (last_pos.y - sight.pos.y) / 3;
            a[tot].lun = 1;
            a[tot].vx = (last_pos.x - sight.pos.x) / 3;
            a[tot].vy = (last_pos.y - sight.pos.y) / 3;
            tot++;
        }
        int maxdis = 0;
        Vec2 tm;
        for (int i = 1; i <= 50; i++) {
            tm.x = rand() % 60 + 1;
            tm.y = rand() % 90 + 1;
            while (!SDK::reachable(tm)) {
                tm.x = rand() % 60 + 1;
                tm.y = rand() % 90 + 1;
            }
            if (dis(sight.pos, tm) >= 20.0) {
                tar = tm;
                duo = 1;
                break;
            }
        }
        actions->emplace(SelectDestination, tar);
        //cout<<duo<<" "<<sight.pos.x<<" "<<sight.pos.y<<" "<<tar.x<<" "<<tar.y<<endl;
        ////cout<<"3"<<endl;
        // //cout<<tar.x<<" "<<tar.y<<endl;
    }
    //cout<<"5 lun"<<endl;
    /*
     * 鞭尸模式
     * 发现一个新出现的尸体，就丢一个炸弹过去
     */

    //连杀猥琐模式（不确定是否更优）
    if (sight.corpseInSightCount) {
        for (int i = 0; i < sight.corpseInSight.size(); i++) {
            int flagco = 0;
            for (int j = 1; j <= tot2; j++)
                if (fabs(b[j].x - sight.corpseInSight[i].x) <= eps && fabs(b[j].y - sight.corpseInSight[i].y) <= eps) {
                    flagco = 1;
                    break;
                }
            if (!flagco) {
                b[++tot2].x = sight.corpseInSight[i].x;
                b[tot2].y = sight.corpseInSight[i].y;
            } else continue;
            for (int j = 1; j <= tot3; j++)
                if (dis2(c[j].pos, sight.corpseInSight[i]) <= 10.0 && (sight.round - c[j].roun) <= 18) {
                    flagco = 1;
                    break;
                }
            if (flagco)continue;
            int yinzi = 0;
            for (int j = 0; j < sight.unitInSight.size(); j++)
                if (dis2(sight.unitInSight[j].pos, sight.corpseInSight[i]) <= 6.0)
                    yinzi++;
            if(yinzi!=1)yinzi*=2;
            for (int j = 0; j < sight.unitInSight.size(); j++)
                if (dis2(sight.unitInSight[j].pos, sight.corpseInSight[i]) <= 6.0)
                    s[sight.unitInSight[j].id].index += (0.4 / (double) yinzi);
        }
    }

    //cout<<"6 lun"<<endl;
    /*
     *  攻击模式
     *  如果周围的人多于3个，A一下，离开
     *  或者如果血量低于一个值，就攻击一个距离自己为x的单位
     *  随机攻击最密集的那个点
     *  然后逃走
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
    if (sight.hp <= 90 || isnight(sight.round) || sight.round <= 20) {
        //ss.Disable();
        Vec2 tmptar;
        tmptar.x = -1;
        if (sight.canSuckAttack) {
            for (int i = 0; i < sight.unitInSight.size(); i++)
                if (dis2(sight.pos, sight.unitInSight[i].pos) <= 4.0 && sight.unitInSight[i].id != sight.id &&
                    s[sight.unitInSight[i].id].index < 0.5) {
                    actions->emplace(SuckAttack, sight.unitInSight[i].id);
                    tmptar = sight.unitInSight[i].pos;
                    c[++tot3].roun = sight.round;
                    c[tot3].pos = sight.unitInSight[i].pos;
                    break;
                }
            for (int i = 0; i < sight.unitInSight.size(); i++)
                if (dis2(sight.pos, sight.unitInSight[i].pos) <= 4.0 && sight.unitInSight[i].id != sight.id) {
                    actions->emplace(SuckAttack, sight.unitInSight[i].id);
                    tmptar = sight.unitInSight[i].pos;
                    c[++tot3].roun = sight.round;
                    c[tot3].pos = sight.unitInSight[i].pos;
                    break;
                }
        }
        if (SDK::reachable(tmptar)) {
            int r = 10, theta;
            float dista = 0;
            for (int i = 1; i <= 50; i++) {
                theta = rand() % 360 + 1;
                Vec2 taoa;
                taoa.x = sight.pos.x + (float) r * sin((float) theta * pi / 180.0);
                taoa.y = sight.pos.y + (float) r * cos((float) theta * pi / 180.0);
                if (SDK::reachable(taoa)) {
                    if (dis(taoa, tmptar) > dista) {
                        tar = taoa;
                        dista = dis(taoa, tmptar);
                    }
                }
            }
            actions->emplace(SelectDestination, tar);
        }  else {
            int idd=0,mxdi=99;
            for(int i=head;i<=tail;i++)
                if(s[i].live&&s[i].index<=0.4){
                if(dis(sight.pos,s[i].pos)<mxdi){
                    idd=i;
                    mxdi=dis(sight.pos,s[i].pos);
                }
            }
            tar=s[idd].pos;
            actions->emplace(SelectDestination, tar);
        }
        //cout<<duo<<" "<<sight.pos.x<<" "<<sight.pos.y<<" "<<tar.x<<" "<<tar.y<<endl;
    } else {
        int idd=0,mxdi=99;
        for(int i=head;i<=tail;i++)
            if(s[i].live&&s[i].index<=0.4){
                if(dis(sight.pos,s[i].pos)<mxdi){
                    idd=i;
                    mxdi=dis(sight.pos,s[i].pos);
                }
            }
        tar=s[idd].pos;
        actions->emplace(SelectDestination, tar);
    }
    //cout<<"7 lun"<<endl;
    /*
     * 真·使用炸弹
     * 用炸弹炸那些指数最高的人
     */
    if (sight.canUseBomb) {
        int jiid = -1, maxnum = 0;
        for (int i = 0; i < sight.unitInSight.size(); i++)
            if (s[sight.unitInSight[i].id].index > 0.6 && sight.unitInSight[i].id != sight.id) {
                int nu = 0;
                for (int j = 0; j < sight.unitInSight.size(); j++)
                    if (dis2(sight.unitInSight[i].pos, sight.unitInSight[j].pos) <= 6.0)
                        nu++;
                if (nu > maxnum) {
                    jiid = sight.unitInSight[i].id;
                    maxnum = nu;
                }
            }
        if (maxnum>=3||isnight(sight.round)) {
            actions->emplace(UseItem, BombItem, s[jiid].pos);
            a[tot].x = sight.pos.x + (s[jiid].pos.x - sight.pos.x) / 3;
            a[tot].y = sight.pos.y + (s[jiid].pos.y - sight.pos.y) / 3;
            a[tot].lun = 2;
            a[tot].vx = (s[jiid].pos.x - sight.pos.x) / 3;
            a[tot].vy = (s[jiid].pos.y - sight.pos.y) / 3;
            tot++;
            c[++tot3].pos = s[jiid].pos;
            c[tot3].roun = sight.round + 3;
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
     * 随机走位略略略
     */
    /*
    int x = 0, y = 0, jx, jy;
    cnt = 9999;
    double di = 9999;
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
     * 鞭尸模式。。。
     */


    /*
     *  反击模式
     *  被攻击后如果有炸弹就原地丢炸弹
     *  并且A一个单位
     */
    ////cout<<"8 lun"<<endl;
    ////cout<<sight.pos.x<<" "<<sight.pos.y<<endl;
    ////cout<<tar.x<<" "<<tar.y<<endl;
    ////cout<<SDK::reachable(tar)<<endl;
    //actions->emplace(SelectDestination, tar);
    last_pos = sight.pos;
    hp_last = sight.hp;
    fclose(stdin);
}

}
