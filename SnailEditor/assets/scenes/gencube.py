import uuid
import random

# 配置
FILENAME = "InstancingTest.snail"
GRID_SIZE = 100  # 100x100 = 10,000 个物体
SPACING = 30.0  # 间距
SCALE_MIN = 15.0
SCALE_MAX = 25.0

# 头部信息 (复制自你的示例，调整了相机位置以俯瞰整个网格)
header = f"""Scene: InstancingTest
SceneSettings:
  AmbientStrength: 0.2
EditorCamera:
  EditorCameraMode: Arcball
  FOV: 45
  Near: 0.1
  Far: 10000
  Aspect: 1.77
  ViewportWidth: 1920
  ViewportHeight: 1080
  ViewMatrix: [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
  Position: [0, 3000, 2000] 
  DirvecFront: [0, -0.8, -0.6]
  DirvecRight: [1, 0, 0]
  DirvecUp: [0, 0.6, -0.8]
  DirvecWorldUp: [0, 1, 0]
  EulerPitch: 45
  DirvecYaw: 0
  FocalPoint: [0, 0, 0]
  Distance: 4000
  RotateSensitivity: 20
  MoveSensitivity: 500
  RotateSpeed: 0.1
  MoveSpeed: 1.0
  ZoomSpeed: 2.0
Entities:
"""

# 定义一个光源
light_entity = f"""  - Entity: {str(uuid.uuid4())}
    TagComponent:
      Tag: Sun
    TransformComponent:
      Position: [0, 1000, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    PointLightComponent:
      Color: [1, 1, 1, 1]
      Intensity: 1.0
    ModelComponent:
      IsImported: false
      ShaderPath: assets/shaders/light_box.glsl
      Meshes:
        - PrimitiveType: Cube
          Textures: []
      Visible: true
      EdgeEnable: false
"""

# 定义大量立方体
cubes = ""
offset = (GRID_SIZE * SPACING) / 2

for x in range(GRID_SIZE):
    for z in range(GRID_SIZE):
        uid = str(uuid.uuid4())

        # 位置居中
        pos_x = (x * SPACING) - offset
        pos_z = (z * SPACING) - offset
        pos_y = random.uniform(-50, 50)  # 高度稍微随机一点

        # 随机旋转 (测试法线矩阵)
        rot_x = random.uniform(0, 360)
        rot_y = random.uniform(0, 360)
        rot_z = random.uniform(0, 360)

        # 统一缩放 (性能优化路径)
        scale_val = random.uniform(SCALE_MIN, SCALE_MAX)

        cube_str = f"""  - Entity: {uid}
    TagComponent:
      Tag: Cube_{x}_{z}
    TransformComponent:
      Position: [{pos_x:.2f}, {pos_y:.2f}, {pos_z:.2f}]
      Rotation: [{rot_x:.2f}, {rot_y:.2f}, {rot_z:.2f}]
      Scale: [{scale_val:.2f}, {scale_val:.2f}, {scale_val:.2f}]
    ModelComponent:
      IsImported: false
      ShaderPath: assets/shaders/Standard.glsl
      Meshes:
        - PrimitiveType: Cube
          Textures: [] 
      Visible: true
      EdgeEnable: false
"""
        cubes += cube_str

# 写入文件
with open(FILENAME, "w", encoding="utf-8") as f:
    f.write(header)
    f.write(light_entity)
    f.write(cubes)

print(f"成功生成 {FILENAME}，包含 {GRID_SIZE * GRID_SIZE + 1} 个实体。")