 

## Assignment 0

**环境配置(Arch Linux)**

依赖库安装

```bash
yay -S eigen opencv2
```

**作业**

- 实现旋转并位移
  $$
  \left[
  \begin{matrix}
  \cos\theta & -\sin\theta & t_x \\
  \sin\theta & \cos\theta & t_y \\
  0 & 0 & 1 \\
  \end{matrix}
  \right] \times P
  $$

**其他**

`eigen`的向量乘法

```cpp
Vector3d v(1,2,3);
Vector3d w(0,1,2);
cout << "Dot product: " << v.dot(w) << endl;//点乘 
cout << "Dot product via a matrix product: " << v.adjoint()*w << endl;//点乘 
cout << "Cross product:\n" << v.cross(w) << endl;//叉乘
```

注意: 叉乘只能用于维数为3的向量, 点乘使用于任何维数的向量。
