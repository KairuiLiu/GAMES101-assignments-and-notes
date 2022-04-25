

## Assignment 1

作业需要使用CPU模拟光栅渲染器, 我们需要完成一个模型变换矩阵与投影矩阵

- 创建一个绕$oz$旋转矩阵
  $$
  \left[\begin{matrix}
  \cos x & -\sin x & 0 & 0\\ \sin x & \cos x & 0 &0\\
  0 & 0 & 1 & 0\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  $$

- 创建一个投影矩阵
  $$
  \left[\begin{matrix}
  n & 0 & 0 & 0\\
  0 & n & 0 &0\\
  0 & 0 & n+f & -nf\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  $$

- 使用罗德格旋转公式完成附加题
  $$
  R(\vec{n},\alpha) = \cos(\alpha)\vec{I} + (1-\cos(\alpha))\vec{n}\vec{n}^T+\sin(\alpha)\left(\begin{matrix}
  0&-n_z&n_y\\n_z&0&-n_x\\-n_y&n_x&0
  \end{matrix}\right)
  $$

结果

- 绕$oz$旋转

  ![绕oz旋转](./result/rotate@(0,0,1).gif)

- 绕$(1,1,1)/\sqrt{3}$旋转

  ![绕(1,1,1)旋转](./result/rotate@(1,1,1).gif)
