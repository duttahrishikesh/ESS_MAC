# -*- coding: utf-8 -*-
"""
Created on Tue Apr 26 22:03:21 2022

@author: hrish
"""



import numpy as np
import math
from random import randint
from random import random
import copy

from matplotlib import pyplot as plt



epochs = 200000






eps_max = 1.0

eps_decay = 10000.0

gamma = 0.99
alpha = 0.1

beta1 = 0.3
beta2 = 0.3

rate = [0.16,0.16,0.16]
fe = [[0],[0],[0]]




alpha_max = 0.1
alpha_min = 0.01
# alpha_min = 0.1
lr_decay = 200.0
# lr_decay = 200.0
eps_decay = 10000.0
# alpha = 0.05

delta_ac = 1.0  #0.9,0.8,0.6

temp = 100000


























# pkt_2_tx[1][:]=np.array([np.random.poisson(lam=rate[0], size=200000).tolist()])


pkt_2_d = []


def moving_average(a, n=20) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

def rolling_window(a, window):
    pad = np.ones(len(a.shape), dtype=np.int32)
    pad[-1] = window-1
    pad = list(zip(pad, np.zeros(len(a.shape), dtype=np.int32)))
    a = np.pad(a, pad,mode='reflect')
    shape = a.shape[:-1] + (a.shape[-1] - window + 1, window)
    strides = a.strides + (a.strides[-1],)
    return np.lib.stride_tricks.as_strided(a, shape=shape, strides=strides)
    

def flow_controller(p_tx,u_del,source, int_node, sn, num_flows):
    
    conv = False
    states = 9
    actions = 1024 #TT,TL,LT,LL  LL,LT,TL,TT
    
    
    
    q_status = [[0],[0],[0]]


    del_ql = [0,0,0]
    af = [0,0,0]
    action = [0,0,0]
    a_status = [[],[],[]]
    delay_list = [[],[],[]]
    delay_total = [[],[],[]]
    est_d = [[],[],[]]
    af_hist = [[],[],[]]
    rf_hist = [[],[],[]]
    dq = [[],[],[]]
    # pkt_del = [[],[],[]]
    mu = [[0,0],[0,0],[0,0]]
    f_list = [[],[],[]]
    state_list = [[0],[0],[0]]
    # index=[0,0,0]
    out_f = [0,0,0]
    del_mu = [0,0,0]
    fair_ind = [0,0,0]
    r = [0,0,0]
    delta= [0,0,0]
    on_frames = [0,0,0]
    off_frame = [0,0,0]
    past_on_frames = [0,0,0]
    on_grad = [0,0,0]
    res_f = [0,0,0]
    mu_var = [0,0,0]
    
    
    
    
    
    
    
    eff_tx = [np.zeros(actions),np.zeros(actions),np.zeros(actions)]
    
    chg_q = [np.zeros(actions),np.zeros(actions),np.zeros(actions)]
    
    act_ct = [np.zeros(actions),np.zeros(actions),np.zeros(actions)]
    

    out_4_rl = int (np.log2(actions))
    
    epochs_f = int(epochs/out_4_rl)
    
    nf = 0
    conv_ac = [0,0,0]
    td = np.zeros((num_flows,epochs_f*out_4_rl))
    p2d = np.zeros((num_flows,epochs_f*out_4_rl))

    while(nf<num_flows):
        
        q_f = 0
        
        Q_table = np.zeros((states,actions))

        state = 0
        
        conv = False

        
        for epochid in range(epochs_f):
            
            if epochid%1000==0 and epochid>0:
                print(nf)
                print(round((epochid-1)/epochs_f,2),' Completed')
            
            
            epsilon = eps_max * math.exp(-(epochid / 1000.0))
            
            rf1 = random()
            
            
            
            
            if conv == False:
                if (rf1 < epsilon):
                    af = randint(0, actions-1)
                else:
                    af = np.argmax(Q_table,1)[state]
                    
                af_hist.append(af)
                    
            else:
                af = conv_ac
                
                af_hist.append(af)

            
            


            for i in range(out_4_rl):            
                
                    
                if len(delay_list[nf])>200:
                
                    res_f[nf] = [x + 1 for x in delay_list[nf]]
                    
                else:
                    
                    res_f[nf] = [x + 1 for x in delay_list[nf]]
                
                delay_list[nf] = copy.deepcopy(res_f[nf])
                
                action = int(np.binary_repr(af, width=out_4_rl)[i])
                
                tmp_ind = i+epochid*out_4_rl
                
                p_4_j = int(p_tx[nf][tmp_ind])
                
                q_f += int(p_tx[nf][tmp_ind])
                
                
                if source==True:
                    if nf==sn:
                        if p_4_j !=0:
                            for it in range(p_4_j):
                                delay_list[nf].append(1)
                    elif int_node==True:
                        if p_4_j !=0:
                            delay_list[nf].append(copy.deepcopy(u_del[nf][tmp_ind]))
                            
                elif int_node==True:
                    if p_4_j !=0:
                        delay_list[nf].append(copy.deepcopy(u_del[nf][tmp_ind]))
                

       
                act_flag=0
                
                if nf!=0:
                    zz = nf-1
                    if p2d[zz][tmp_ind]==1:
                        act_flag=1
                
                if action==1 and q_f>0 and act_flag==0:
                    q_f-= 1
                    pkt_2_d.append(1)
                    p2d[nf][tmp_ind]=1
                    # if len(delay_list[j])!=0:
                    delay_total[nf].append(copy.deepcopy(delay_list[nf].pop(0)))
                    td[nf][tmp_ind]=copy.deepcopy(delay_total[nf][-1])
                else:
                    pkt_2_d.append(0)
                    delay_total[nf].append(-100)
                    td[nf][tmp_ind]=copy.deepcopy(delay_total[nf][-1])
                
                


                # if action[j]==0 and q_f[j]>0:
                #     delay_total[j].append(copy.deepcopy(delay_list[j][0]))
                    
                
                if delay_list[nf]:
                    est_d[nf].append(np.mean(delay_list[nf]))
                else:
                    est_d[nf].append(0)
                      
                q_status[nf].append(copy.deepcopy(q_f))
                
                a_status[nf].append(copy.deepcopy(action))
                
            
                
            
            del_ql[nf] = q_status[nf][-1]-q_status[nf][-out_4_rl+1]
            dq[nf].append(copy.deepcopy(del_ql[nf])) 
            
            if del_ql[nf]>=0 and del_ql[nf]<=3:
                state=int(del_ql[nf])
            if del_ql[nf]>=4:
                state=4
            if del_ql[nf]<0 and del_ql[nf]>=-3:
                state=int(abs(del_ql[nf])+4)
            if del_ql[nf]<-4:
                state=8
                
            state_list[nf].append(state)
        
            out_f[nf] = (1/out_4_rl)*(sum(a_status[nf][-out_4_rl:-1])+a_status[nf][-1])
            
            fe[nf].append(abs(out_f[nf]-rate[nf]))
            
            del_mu[nf]= mu[nf][-1]-mu[nf][-2]
            
            mu[nf].append(copy.deepcopy(out_f[nf]))
                
            if conv == False:
                
                
                on_frames[nf] = a_status[nf][-1]+sum(a_status[nf][-out_4_rl:-1])
                
                off_frame[nf] = out_4_rl-on_frames[nf]
                
                on_grad[nf] = on_frames[nf]-past_on_frames[nf]
                
                past_on_frames[nf] = copy.deepcopy(on_frames[nf])
                
                act_ct[nf][af]+=1
                
                eff_tx[nf][af]=off_frame[nf]
                
                chg_q[nf][af]=(chg_q[nf][af]+del_ql[nf])/act_ct[nf][af]
            
                
                
                fctr=0.01*(math.exp((epochid / 1000)))
                
                r=-3*fctr*chg_q[nf][af]*eff_tx[nf][af]
                
                
                # if (on_grad[j]<=0 and (out_f[j]-rate[j])>0.00) and out_f[j]!=0 and  fair_ind[j]<=0:
                #     r[j] = 1
                # else:
                #     r[j] = -1
                
            
                delta = r+gamma*np.max(Q_table[state_list[nf][-2]])-Q_table[state_list[nf][-2]][af]
                
                if delta>=0:
                    alpha = beta1
                else:
                    alpha = beta2
                    
                    
                Q_table[state_list[nf][-2]][af]=Q_table[state_list[nf][-2]][af]+alpha*delta
                
                
                rf_hist[nf].append(r)
            
            
                    
                    
                 
                    
                mu[nf].append(copy.deepcopy(out_f[nf]))
                
                f_list[nf].append(fair_ind[nf])
                
                mu_var[nf]=np.var(rolling_window(np.array(mu[nf]), 100), axis=-1)
        
                if mu_var[nf][-1]<=0.007 and conv == False:
                    conv = True
                    print('Converged')
                    conv_ac = copy.deepcopy(np.argmax(Q_table,1)[state])
                    
        nf+=1
        
    pk_del = [[],[],[]]

    for x in range(len(delay_total)):
        pk_del[x] = [y for y in delay_total[x] if y>=0]

                
    tx_metric = np.array([af_hist[0],mu[0],q_status[0],pk_del[0]])
    return p2d, td, tx_metric
                





def listen(pkt4s,pktdu, slot_ls, f_s):
    
    arms = 2
    

    agents = f_s-1
    
    
    epochs_l = epochs
    
    pkt_cnt = np.zeros(agents)
    on_cnt = np.zeros(agents)
    est_pkt_cnt = np.zeros(agents)
    idle_cnt = np.zeros(agents)
    value = np.zeros((agents,arms))
    value_lr = np.zeros(agents)
    value_rew = np.zeros(agents)

    
    p4s = np.zeros((f_s-1,epochs_l))
    
    pdu = np.full((f_s-1,epochs_l), -100)
    
    for s,t in enumerate(slot_ls):    
        p4s[t][0:np.shape(pkt4s[s])[1]]=pkt4s[s]
        pdu[t][0:np.shape(pkt4s[s])[1]]=pktdu[s]
    
    
    # pkt_per_frame = sum(p4s).tolist()
    
    
    action_replay = [[],[],[],[],[],[]]
    
    q = np.zeros(agents)
    rew = np.zeros(agents)
    
    drop_list = []
    rcvd_list = []
    awake_list = []
    idle_list = []
    Q_mab = np.zeros((agents,arms))
    
    true_sleep_list = []
    no_pkt_list = []


    p2d = np.zeros((f_s-1,epochs_l))
    
    for epochid in range(epochs_l):
        
        drop=0
        rcvd = 0
        awake = 0
        idle_slot = 0
        
        no_pkt = 0
        true_sleep = 0
        
        epsilon = eps_max * math.exp(-(epochid / eps_decay))
        
        
        
        
        
        if epochid<100000:
            l=alpha_min*(math.exp((epochid / lr_decay)))
        
        
        if epochid%1000==0:
            print(epochid/epochs_l)
        
        
        # l=0.9
        
        
        if l<alpha_max:
            alpha = copy.deepcopy(l)
        else:
            alpha = alpha_max
        
        
        
        for slotid in range(agents):
            
            
            
            rf1 = random()
            
            
            
            if rf1<epsilon:
                action_f1 = randint(0, arms-1)
            else:
                rf2 = random()
                
                if value[slotid][1]>0.001:
                    # tx_p = value[slotid][1]+0.2
                    tx_p = value[slotid][1]*(1+delta_ac*(1-value[slotid][1])/value[slotid][1])
                else:
                    tx_p = value[slotid][1]
                
                if rf2<tx_p:
                    action_f1 = 1
                else:
                    action_f1 = 0
                
            
            action_replay[slotid].append(action_f1)
            
                
            if action_f1==1:
                q[slotid] += p4s[slotid][epochid]
                on_cnt[slotid]+=1
                awake+=1
                p2d[slotid][epochid]=p4s[slotid][epochid]
        
        
            if on_cnt[slotid]!=0:
                value_lr[slotid]=1/on_cnt[slotid]
            
            
        
        
            if action_f1==1 and p4s[slotid][epochid]==1:
                pkt_cnt[slotid]+=1
        
        
            if action_f1==1 and p4s[slotid][epochid]==1:
                value_rew[slotid]=1
            if action_f1==1 and p4s[slotid][epochid]==0:
                value_rew[slotid]=0            
                idle_cnt[slotid]+=1
                idle_slot+=1
                no_pkt+=1
            if action_f1==0:
                if on_cnt[slotid]!=0:
                    value_rew[slotid]=idle_cnt[slotid]/on_cnt[slotid]
                else:
                    value_rew[slotid]=0
                
                
            value[slotid][action_f1]=value[slotid][action_f1]+value_lr[slotid]*(value_rew[slotid]-value[slotid][action_f1])
        
            
        
        
            if action_f1==0 and p4s[slotid][epochid]==1:
                drop+=1
                pdu[slotid][epochid]=-100
                
            if action_f1==0 and p4s[slotid][epochid]==0:
                true_sleep+=1
                no_pkt+=1
                
            if action_f1==1 and p4s[slotid][epochid]==1:
                rcvd+=1
            
        
        
        
        
            if on_cnt[slotid]!=0:
                est_pkt_cnt[slotid]=pkt_cnt[slotid]/on_cnt[slotid] 
        
            if action_f1==1 and p4s[slotid][epochid]==1:
                rew[slotid]=1
            if action_f1==1 and p4s[slotid][epochid]==0:
                rew[slotid]=-1
            if action_f1==0:
                rew[slotid]=idle_cnt[slotid]/on_cnt[slotid]
                
                
            Q_mab[slotid][action_f1]=Q_mab[slotid][action_f1]+alpha*(rew[slotid]-Q_mab[slotid][action_f1])
        

        drop_list.append(drop)
        rcvd_list.append(rcvd)
        awake_list.append(awake)
        idle_list.append(idle_slot)
        true_sleep_list.append(true_sleep)
        no_pkt_list.append(no_pkt)




    # dl = [y/(rcvd_list[x]+y) for x,y in enumerate(drop_list) if rcvd_list[x]!=0]



    idle_list_prob = [x/f_s for x in idle_list]

    off_prob = [1-(x/f_s) for x in awake_list]



    eff_set_based = [(true_sleep_list[x]-drop_list[x])/y for x, y in enumerate(no_pkt_list) if y!=0]


    metric = np.array([idle_list_prob,off_prob,true_sleep_list,drop_list,no_pkt_list, eff_set_based])
    
    return  p2d, pdu, metric

    





num_nodes = 5
frame_size = 4
sources = [1,4]
int_nodes = [2]
destinations = [3,5]

alloc_slot = [0,2,3,1,0]


upstream = [ [] for _ in range(num_nodes) ]
downstream  = [ [] for _ in range(num_nodes) ]

flow = [[1,2,3],[4,2,5]]

for i in range(len(flow)):
    for j in range(1,len(flow[i])):
        upstream[flow[i][j]-1].append(flow[i][j-1])
        
for i in range(len(flow)):
    for j in range(len(flow[i])-1):
        downstream[flow[i][j]-1].append(flow[i][j+1])
        
        
       
pkt_2_tx = np.array([np.random.poisson(lam=rate[0], size=200000).tolist()])
upstream_del = np.array([np.random.poisson(lam=rate[0], size=200000).tolist()])


fc_perf = [[],[],[]]
ls_perf = np.zeros((3,epochs))

pkt_out_tx = []
pkt_out_tx_del = []
pkt_out_tx_metric = []


pkt_out_rx = []
pkt_out_rx_del = []
pkt_out_rx_metric = []


pot = {}
potd = {}
potm = {}


por = {}
pord = {}
porm = {}


for x, y in enumerate (sources):
    mm1, mm2, mm3 = flow_controller(pkt_2_tx,upstream_del, source=True, int_node=False,sn=0,num_flows = 1)

    pkt_out_tx.append(copy.deepcopy(mm1))
    pkt_out_tx_del.append(copy.deepcopy(mm2))
    pkt_out_tx_metric.append(copy.deepcopy(mm3))
    
    
    dest_p = {}
    dest_d = {}
    
    for nm, nn in enumerate(downstream[y-1]):
        dest_p[nn] = copy.deepcopy(mm1[nm])
        dest_d[nn] = copy.deepcopy(mm2[nm])
        
    pot[y] = dest_p
    potd[y] = dest_d
    potm[y] = copy.deepcopy(mm3)
    

for x,y in enumerate(int_nodes):
    
    n_up = len(upstream[y-1])
    
    temp_pkt_out_tx = []
    temp_pkt_out_tx_del = []
    
    slt_lst = []
    
    for nn in upstream[y-1]:
        temp_pkt_out_tx.append(np.expand_dims(pot[nn][y], axis = 0))
        temp_pkt_out_tx_del.append(np.expand_dims(pot[nn][y], axis = 0))
        slt_lst.append(alloc_slot[nn-1])
    
    mm1, mm2, mm3 = listen(temp_pkt_out_tx, temp_pkt_out_tx_del, slt_lst, frame_size)
    
    pkt_out_rx.append(copy.deepcopy(mm1))
    pkt_out_rx_del.append(copy.deepcopy(mm2))
    pkt_out_rx_metric.append(copy.deepcopy(mm3))
    
    por[y] = copy.deepcopy(mm1)
    pord[y] = copy.deepcopy(mm2)
    porm[y] = copy.deepcopy(mm3)
    
    
    mm1,mm2,mm3 = flow_controller(por[y], por[y], source=False, int_node=True,sn=1,num_flows = n_up)
            
    pkt_out_tx.append(copy.deepcopy(mm1))
    pkt_out_tx_del.append(copy.deepcopy(mm2))
    pkt_out_tx_metric.append(copy.deepcopy(mm3))
    
    
    dest_p = {}
    dest_d = {}
    
    for nm, nn in enumerate(downstream[y-1]):
        dest_p[nn] = copy.deepcopy(mm1[nm])
        dest_d[nn] = copy.deepcopy(mm2[nm])
    
    pot[y] = dest_p
    potd[y] = dest_d
    potm[y] = copy.deepcopy(mm3)
        
    
    
    
for x, y in enumerate(destinations):
    temp_pkt_out_tx = []
    temp_pkt_out_tx_del = []
    
    slt_lst = []
    
    for nn in upstream[y-1]:
        temp_pkt_out_tx.append(np.expand_dims(pot[nn][y], axis = 0))
        temp_pkt_out_tx_del.append(np.expand_dims(pot[nn][y], axis = 0))
        slt_lst.append(alloc_slot[nn-1])
    
    mm1, mm2, mm3 = listen(temp_pkt_out_tx, temp_pkt_out_tx_del, slt_lst, frame_size)
    
    pkt_out_rx.append(copy.deepcopy(mm1))
    pkt_out_rx_del.append(copy.deepcopy(mm2))
    pkt_out_rx_metric.append(copy.deepcopy(mm3))
    
    por[y] = copy.deepcopy(mm1)
    pord[y] = copy.deepcopy(mm2)
    porm[y] = copy.deepcopy(mm3)
        
        
        
        







plt.figure(1)
plt.plot(moving_average(porm[2][3],10000))
plt.plot(moving_average(porm[3][3],10000))
plt.plot(moving_average(porm[5][3],10000))
plt.xlabel('Epochs')
plt.ylabel('Packet Drop probability')
    
