## Lecture 2 线性代数复习

**图形学需要知识**

- 线性代数/ 微积分/ 统计学
- 光学/ 力学
- 信号处理/ 数值分析

**向量**

- 向量只有长度与方向没有绝对的始终点. 加减遵循平行四边形法则

- 默认使用列向量, 也可以写作$(x,y,z)^T$

- 用$\| \vec{a} \|$表示向量长度

- 用$\hat{a}$表示与$a$向量同方向的单位向量, 即$\hat{a} = \vec{a} / \| \vec{a}\|$

- 向量点乘

  - $\vec{a} \cdot \vec{b} = \|\vec{a}\|\|\vec{b}\|\cos\theta$, 即$\cos\theta = \hat{a}\cdot\hat{b}$

  - 在笛卡尔坐标系下
    $$
    \vec{a} \cdot \vec{b} = \left(\begin{matrix}x_a\\ y_a\end{matrix}\right) \cdot \left(\begin{matrix}x_b\\ y_b\end{matrix}\right) = x_ax_b+y_ay_b
    $$

    $$
    \vec{a} \cdot \vec{b} = \left(\begin{matrix}x_a\\ y_a\\ z_a\end{matrix}\right) \cdot \left(\begin{matrix}x_b\\ y_b\\ z_b\end{matrix}\right) = x_ax_b+y_ay_b+z_az_b
    $$

  - 向量点乘满足交换与分配率

  - 应用: 求两向量夹角 / 求向量在另一个向量上的投影 / 两个向量之间的垂直距离(投影向量减一下) / 描述两个向量有多接近 / 描述两个向量的前后关系(点积正负)

- 向量叉积

  - 输入两个向量$\vec{a}, \vec{b}$, 得到与两个向量都垂直的向量

    - 大小: $\|\vec{a}\| \|\vec{b}\| \sin\theta$
    - 方向: 使用右手螺旋法则从$\vec{a}$握到$\vec{b}$, 拇指指向

  - 在笛卡尔坐标系中
    $$
    \vec{a} \times \vec{b} = \left(\begin{matrix}x_a\\ y_a\\ z_a\end{matrix}\right) \times \left(\begin{matrix}x_b\\ y_b\\ z_b\end{matrix}\right) = \left(\begin{matrix}y_az_b-y_bz_a\\ z_ax_b-z_bx_a\\ x_ay_b-x_by_a\end{matrix}\right)
    $$
    矩阵表示
    $$
    \vec{a} \times \vec{b} = A^* b = \left(\begin{matrix}0&-z_a&y_a\\z_a&0&-x_a\\-y_a&x_a&0\end{matrix}\right) \left(\begin{matrix}x_b\\y_b\\z_b\end{matrix}\right)
    $$
    注意矩阵表示时$A^*$是一个整体表示dual matrix, 不要当成乘号

  - 不满足交换率( $\vec{a} \times \vec{b} = - \vec{b} \times \vec{a}$ ), 满足分配率

  - 应用: 

    - 建立三维直角坐标系(右手系)
    - 判定左右: $\vec{a} \times \vec{b} > 0$说明$\vec{b}$在$\vec{a}$左边
    - 判断点在多边形内: 以三角形为例, 判断点$P$在三角形$ABC$内即判断$\vec{AB} \times \vec{AP}$, $\vec{BC} \times \vec{BP}$, $\vec{CA} \times \vec{CP}$是否同号(顶点枚举顺序不重要), 我们称叉积为$0$的情况为conner case, 随你把他定义为在内部还是外部

  

  **矩阵**

  - 矩阵乘法中结果矩阵的$m$行$n$列就是第一个矩阵的$m$行乘第二个矩阵$n$列(虽然有点废话, 但是蛮好用的)
  - 矩阵乘法没有交换率, 有结合率与分配率. $(AB)^T = B^TA^T$
  - 应用: 对图像中的点应用变换

