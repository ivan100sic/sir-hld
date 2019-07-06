#include <vector>
#include <iostream>
#include <string>
using namespace std;

struct hld_basic {
	vector<vector<int>> e;
	int root;
	
	vector<int> p, h, d, r, pos, c;

	void init(const vector<vector<int>>& e_, int root_ = 0) {
		e = e_;
		root = root_;
		int n = e.size();
		p = h = d = r = pos = c = vector<int>(n, -1);

		d[root] = 0;
		dfs(root);
		finish();
	}

	int dfs(int x) {
		int shx = -1, sx = 1;
		for (int y : e[x]) {
			if (y != p[x]) {
				p[y] = x;
				d[y] = d[x] + 1;
				int sy = dfs(y);
				sx += sy;
				if (sy > shx) {
					shx = sy;
					h[x] = y;
				}
			}
		}
		return sx;
	}

	void finish() {
		int n = e.size(), k = 0;
		for (int i=0; i<n; i++) {
			if (i == root || h[p[i]] != i) {
				for (int j=i; j!=-1; j=h[j]) {
					r[j] = i;
					c[k] = j;
					pos[j] = k++;
				}
			}
		}
	}

	int kth_ancestor(int x, int k) {
		if (d[x] < k)
			return -1;
		while (k > 0) {
			if (k <= d[x] - d[r[x]]) {
				return c[pos[x]-k];
			} else {
				k -= d[x] - d[r[x]] + 1;
				x = p[r[x]];
			}
		}
		return x;
	}

	int lca(int x, int y) {
		while (r[x] != r[y]) {
			if (d[r[x]] > d[r[y]])
				swap(x, y);
			y = p[r[y]];
		}
		return d[x] < d[y] ? x : y;
	}
};

template<class T>
struct segment_tree {
	vector<T> a;
	int maxn;

	segment_tree(int n) {
		maxn = 1;
		while (maxn < n)
			maxn <<= 1;
		a.resize(2*maxn);
	}

	void update(int x, T y) {
		x += maxn;
		a[x] = y;
		while (x > 1) {
			x >>= 1;
			a[x] = a[2*x] + a[2*x+1];
		}
	}

	T query(int l, int r, int x, int xl, int xr) {
		if (r < xl || xr < l)
			return T();
		if (l <= xl && xr <= r)
			return a[x];
		int xm = (xl+xr) >> 1;
		return
			query(l, r, 2*x, xl, xm) +
			query(l, r, 2*x+1, xm+1, xr);
	}

	T query(int l, int r) {
		return query(l, r, 1, 0, maxn-1);
	}
};

template<class T>
struct hld : hld_basic {
	segment_tree<T> tree;

	hld(const vector<vector<int>>& e, int root = 0)
		: tree(e.size())
	{
		init(e, root);
	}

	void update(int x, T y) {
		tree.update(pos[x], y);
	}

	T query(int x, int y) {
		T sum = T();
		while (r[x] != r[y]) {
			if (d[r[x]] > d[r[y]])
				swap(x, y);
			sum = sum + tree.query(pos[r[y]], pos[y]);
			y = p[r[y]];
		}
		if (d[x] > d[y])
			swap(x, y);
		return sum + tree.query(pos[x], pos[y]);
	}
};

struct max_int {
	int x;

	max_int(int x = -2e9) : x(x) {}

	explicit operator int() const { return x; }

	max_int operator+ (max_int b) const {
		return max(x, b.x);
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	int t;
	cin >> t;
	while (t--) {
		int n;
		cin >> n;
		vector<vector<int>> e(2*n-1);
		vector<int> costs(n-1);
		for (int i=n; i<2*n-1; i++) {
			int u, v;
			cin >> u >> v >> costs[i-n];
			u--, v--;
			e[i].push_back(u);
			e[u].push_back(i);
			e[i].push_back(v);
			e[v].push_back(i);
		}
		hld<max_int> a(e);
		for (int i=n; i<2*n-1; i++)
			a.update(i, costs[i-n]);
		while (1) {
			string cmd;
			cin >> cmd;
			if (cmd == "DONE")
				break;
			else if (cmd == "QUERY") {
				int u, v;
				cin >> u >> v;
				u--, v--;
				cout << (int)a.query(u, v) << '\n';
			} else {
				int x, y;
				cin >> x >> y;
				a.update(x+n-1, y);
			}
		}
	}
}
