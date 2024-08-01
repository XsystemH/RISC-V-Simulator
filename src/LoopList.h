//
// Created by 13199 on 2024/7/29.
//

#ifndef RISC_V_LOOPLIST_H
#define RISC_V_LOOPLIST_H

template<typename T, unsigned int N>
struct LoopList {
  T arr[N];
  unsigned int head, tail;
  void initialize() {
    head = tail = 0;
    for (int i = 0; i < N; i++) {
      arr[i].busy = false;
    }
  }
  bool full() {
    return head == tail && arr[head].busy;
  }
  bool empty() {
    return head == tail && !arr[head].busy;
  }
  int push(T x) {
    int pos = tail;
    arr[tail++] = x;
    if (tail == N) tail = 0;
    return pos;
  }
  void pop() {
    arr[head].busy = false;
    if (++head == N) head = 0;
//    while (head != tail && !arr[head].busy) {
//      if (++head == N) head = 0;
//    }
  }
  T &operator[](unsigned int x) { return arr[x]; }
  void clear() {
    head = tail = 0;
    for (auto &x: arr) x.busy = false;
  }
};

#endif //RISC_V_LOOPLIST_H
