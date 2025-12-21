import uuid
import random

# 配置

GRID_SIZE = 100
FILENAME = "InstancingTest_" + (pow(GRID_SIZE/100, 2) * 10).__str__() + "K.snl"
SPACING = 30.0  # 间距
SCALE_MIN = 15.0
SCALE_MAX = 25.0

print(f"开始生成 {GRID_SIZE * GRID_SIZE} 个物体的场景...")

# 头部信息
header = f"""Scene: InstancingTest
SceneSettings:
  AmbientStrength: 0.2
EditorCamera:
  EditorCameraMode: Arcball
  FOV: 45
  Near: 0.1
  Far: 50000
  Aspect: 1.77
  ViewportWidth: 1920
  ViewportHeight: 1080
  ViewMatrix: [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
  Position: [0, 5000, 5000] 
  DirvecFront: [0, -0.7, -0.7]
  DirvecRight: [1, 0, 0]
  DirvecUp: [0, 0.7, -0.7]
  DirvecWorldUp: [0, 1, 0]
  EulerPitch: 45
  DirvecYaw: 0
  FocalPoint: [0, 0, 0]
  Distance: 8000
  RotateSensitivity: 20
  MoveSensitivity: 1000
  RotateSpeed: 0.1
  MoveSpeed: 5.0
  ZoomSpeed: 5.0
Entities:
"""

# 定义一个光源
light_entity = f"""  - Entity: {str(uuid.uuid4())}
    TagComponent:
      Tag: Sun
    TransformComponent:
      Position: [0, 2000, 0]
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

# 直接打开文件进行流式写入
with open(FILENAME, "w", encoding="utf-8") as f:
    # 1. 写入头
    f.write(header)
    f.write(light_entity)

    offset = (GRID_SIZE * SPACING) / 2

    # 2. 循环生成并直接写入
    count = 0
    for x in range(GRID_SIZE):
        # 优化：每行生成一个大的 chunk 写入，减少 I/O 调用次数
        chunk = []
        for z in range(GRID_SIZE):
            uid = str(uuid.uuid4())

            pos_x = (x * SPACING) - offset
            pos_z = (z * SPACING) - offset
            pos_y = random.uniform(-50, 50)

            rot_x = random.uniform(0, 360)
            rot_y = random.uniform(0, 360)
            rot_z = random.uniform(0, 360)

            scale_val = random.uniform(SCALE_MIN, SCALE_MAX)

            # 使用 f-string 依然很快
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
            chunk.append(cube_str)
            count += 1

        # 将这一行的 1000 个物体一次性写入磁盘
        f.write("".join(chunk))

        # 进度条
        if x % 10 == 0:
            print(f"进度: {x}/{GRID_SIZE} 行 ({count} 个物体)")

print(f"完成！成功生成 {FILENAME}，包含 {count + 1} 个实体。")