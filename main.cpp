#include <bits/stdc++.h>
using namespace std;

typedef unsigned long long ull;

struct Node {
    int id, left, right, parent, size;
    vector<ull> bits, sub;
    Node() : id(0), left(0), right(0), parent(0), size(0) {}
};

int n, k, m, W, root;
vector<Node> tr;

void setBit(vector<ull>& v, int pos, bool value) {
    ull mask = 1ULL << (pos & 63);
    if (value) v[pos >> 6] |= mask;
    else v[pos >> 6] &= ~mask;
}

bool getBit(const vector<ull>& v, int pos) {
    return (v[pos >> 6] >> (pos & 63)) & 1ULL;
}

void pull(int x) {
    Node& node = tr[x];
    node.size = 1;
    if (node.left) node.size += tr[node.left].size;
    if (node.right) node.size += tr[node.right].size;

    node.sub = node.bits;
    if (node.left) {
        const vector<ull>& v = tr[node.left].sub;
        for (int i = 0; i < W; ++i) node.sub[i] |= v[i];
    }
    if (node.right) {
        const vector<ull>& v = tr[node.right].sub;
        for (int i = 0; i < W; ++i) node.sub[i] |= v[i];
    }
}

void rotateNode(int x) {
    int p = tr[x].parent;
    int g = tr[p].parent;

    if (tr[p].left == x) {
        int child = tr[x].right;
        tr[p].left = child;
        tr[x].right = p;
        if (child) tr[child].parent = p;
    } else {
        int child = tr[x].left;
        tr[p].right = child;
        tr[x].left = p;
        if (child) tr[child].parent = p;
    }

    tr[p].parent = x;
    tr[x].parent = g;

    if (g) {
        if (tr[g].left == p) tr[g].left = x;
        else if (tr[g].right == p) tr[g].right = x;
    }

    pull(p);
    pull(x);
}

void splay(int x) {
    while (tr[x].parent) {
        int p = tr[x].parent;
        int g = tr[p].parent;

        if (g) {
            bool pLeft = (tr[g].left == p);
            bool xLeft = (tr[p].left == x);
            rotateNode(pLeft == xLeft ? p : x);
        } else {
            rotateNode(x);
        }
    }
}

int mergeTree(int a, int b) {
    if (!a) return b;
    if (!b) return a;

    int x = a;
    while (tr[x].right) x = tr[x].right;

    splay(x);
    tr[x].right = b;
    tr[b].parent = x;
    pull(x);
    return x;
}

void detach(int x) {
    splay(x);

    int l = tr[x].left;
    int r = tr[x].right;

    if (l) tr[l].parent = 0;
    if (r) tr[r].parent = 0;

    tr[x].left = tr[x].right = tr[x].parent = 0;
    pull(x);

    root = mergeTree(l, r);
}

void moveTop(int x) {
    detach(x);
    root = mergeTree(x, root);
}

void moveBottom(int x) {
    detach(x);
    root = mergeTree(root, x);
}

void replaceBitmap(int x, vector<ull>& b) {
    splay(x);
    root = x;
    tr[x].bits.swap(b);
    pull(x);
}

void toggleCell(int x, int r, int c) {
    int pos = (r - 1) * n + (c - 1);

    splay(x);
    root = x;
    tr[x].bits[pos >> 6] ^= 1ULL << (pos & 63);
    pull(x);
}

vector<ull> rotateRight(const vector<ull>& src) {
    vector<ull> dst(W, 0);

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (getBit(src, r * n + c)) {
                setBit(dst, c * n + (n - 1 - r), true);
            }
        }
    }

    return dst;
}

vector<ull> mirrorHorizontal(const vector<ull>& src) {
    vector<ull> dst(W, 0);

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (getBit(src, r * n + c)) {
                setBit(dst, r * n + (n - 1 - c), true);
            }
        }
    }

    return dst;
}

long long queryVisible(int x) {
    splay(x);
    root = x;

    int left = tr[x].left;
    long long ans = 0;

    for (int i = 0; i < W; ++i) {
        ull covered = left ? tr[left].sub[i] : 0ULL;
        ans += __builtin_popcountll(tr[x].bits[i] & ~covered);
    }

    return ans;
}

int buildTree(const vector<int>& ids, int l, int r, int parent) {
    if (l > r) return 0;

    int mid = (l + r) >> 1;
    int x = ids[mid];

    tr[x].parent = parent;
    tr[x].left = buildTree(ids, l, mid - 1, x);
    tr[x].right = buildTree(ids, mid + 1, r, x);
    pull(x);

    return x;
}

void printFinalView() {
    vector<int> view(m, 0);
    vector<ull> covered(W, 0);
    vector<int> stk;

    int cur = root;

    while (cur || !stk.empty()) {
        while (cur) {
            stk.push_back(cur);
            cur = tr[cur].left;
        }

        int x = stk.back();
        stk.pop_back();

        for (int i = 0; i < W; ++i) {
            ull add = tr[x].bits[i] & ~covered[i];
            if (!add) continue;

            covered[i] |= tr[x].bits[i];

            while (add) {
                int b = __builtin_ctzll(add);
                int pos = i * 64 + b;
                if (pos < m) view[pos] = tr[x].id;
                add &= add - 1;
            }
        }

        cur = tr[x].right;
    }

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (c) cout << ' ';
            cout << view[r * n + c];
        }
        cout << '\n';
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> k;

    m = n * n;
    W = (m + 63) >> 6;

    tr.resize(k + 1);

    for (int x = 1; x <= k; ++x) {
        tr[x].id = x;
        tr[x].left = tr[x].right = tr[x].parent = 0;
        tr[x].size = 1;
        tr[x].bits.assign(W, 0);
        tr[x].sub.assign(W, 0);

        for (int r = 0; r < n; ++r) {
            string s;
            cin >> s;

            for (int c = 0; c < n; ++c) {
                if (s[c] == '#') {
                    setBit(tr[x].bits, r * n + c, true);
                }
            }
        }
    }

    vector<int> ids;
    ids.reserve(k);

    for (int x = k; x >= 1; --x) ids.push_back(x);

    root = buildTree(ids, 0, k - 1, 0);

    int q;
    cin >> q;

    while (q--) {
        char op;
        cin >> op;

        if (op == 'R') {
            int x;
            cin >> x;
            vector<ull> nxt = rotateRight(tr[x].bits);
            replaceBitmap(x, nxt);
        } else if (op == 'M') {
            int x;
            cin >> x;
            vector<ull> nxt = mirrorHorizontal(tr[x].bits);
            replaceBitmap(x, nxt);
        } else if (op == 'C') {
            int x, r, c;
            cin >> x >> r >> c;
            toggleCell(x, r, c);
        } else if (op == 'U') {
            int x;
            cin >> x;
            moveTop(x);
        } else if (op == 'D') {
            int x;
            cin >> x;
            moveBottom(x);
        } else if (op == 'Q') {
            int x;
            cin >> x;
            cout << queryVisible(x) << '\n';
        }
    }

    printFinalView();
    return 0;
}