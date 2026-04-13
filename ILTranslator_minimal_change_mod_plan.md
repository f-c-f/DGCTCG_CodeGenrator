# ILTranslator 最小侵入式增强修改文档

本文档的原则是：

- 尽量不改原有功能
- 不改变现有 `ILTranslator` 各后端的语义
- 不重写现有 `CBMC` / `TCGStateSearch` 调用链
- 新能力优先采用“旁路导出”“附加模块”“可开关启用”的方式接入

目标仅聚焦三部分：

1. 导出 JSON/CSV 元数据
2. 做 IL-based slicer
3. 增加大模型分析器


## 第一部分：导出 JSON/CSV 元数据

### 1.1 目标

在不改变现有代码生成结果的前提下，为后续分析系统补充结构化元数据输出。

这些元数据不参与当前编译和运行逻辑，只作为附加产物存在。

### 1.2 原则

- 不修改现有生成代码的语义
- 不改 `branchBitmap`、`CondAndMCDCRecord`、`stateBranchList` 的现有使用方式
- 不改变 `main.c`、`main_2.c`、`main.cpp` 的内容语义
- 新增逻辑应只负责“记录并导出”
- 优先通过新类或新工具函数实现

### 1.3 建议新增文件

建议在 [D:\CodeGenerator2\Src\ILTranslator](D:\CodeGenerator2\Src\ILTranslator) 下新增：

- [ILTransMetadataExporter.h](/D:/CodeGenerator2/Src/ILTranslator/ILTransMetadataExporter.h)
- [ILTransMetadataExporter.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTransMetadataExporter.cpp)

建议新增一个轻量数据结构文件：

- [ILTransMetadataModel.h](/D:/CodeGenerator2/Src/ILTranslator/ILTransMetadataModel.h)

### 1.4 建议导出的元数据内容

#### A. branch obligation 元数据

每条 branch obligation 建议导出：

- `bitmap_index`
- `obligation_type`
- `function_name`
- `file_name`
- `condition_text`
- `branch_depth`
- `call_depth`
- `related_inputs`
- `related_globals`
- `cbmc_property_label`

#### B. MCDC / condition 元数据

每个 condition atom 建议导出：

- `cond_id`
- `decision_expr`
- `atom_expr`
- `function_name`
- `file_name`
- `related_inputs`
- `related_globals`

#### C. branch relation 元数据

从现有 `TCGStateSearch` 分支读写分析中导出：

- `branch_id`
- `read_globals`
- `write_globals`
- `parent_branch`
- `children_branch`

#### D. distance 元数据

从 branch distance / array distance 注册逻辑中导出：

- `distance_id`
- `branch_id`
- `distance_type`
- `var_name`
- `compare_op`
- `target_value`

### 1.5 输出文件建议

每次翻译时，在输出目录额外生成：

- `obligation_map.json`
- `obligation_map.csv`
- `branch_relation.json`
- `distance_map.json`

（当前实现为控制产物数量，将 MCDC 与 cond/dec 位图说明合并进 `obligation_map.json` 的 `mcdc_decisions`、`condition_pairs`、`decision_pairs` 字段，不再单独生成 `mcdc_map.json` 等文件。）

如果只先做最小版本，第一阶段只落：

- `obligation_map.json`
- `branch_relation.json`

### 1.6 建议接入点

尽量选择“现有编号已经产生”的地方做旁路记录。

#### 接入点 1：Branch Coverage

文件：

- [ILTransInstrumentBranchCoverage.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTransInstrumentBranchCoverage.cpp)

原因：

- 这里已经生成 `branchBitmap` 编号
- 能拿到 branch / statement 对象
- 能拿到分支条件文本或上下文

建议方式：

- 保持现有插桩逻辑不动
- 在编号分配后调用 `ILTransMetadataExporter::recordBranch(...)`

#### 接入点 2：MCDC

文件：

- [ILTransInstrumentMCDCCoverage.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTransInstrumentMCDCCoverage.cpp)

建议方式：

- 不改现有 `CondAndMCDCRecord` 逻辑
- 在 atom 拆分或 decision 处理时额外记录元数据

#### 接入点 3：CBMC property label

文件：

- [ILTransMainForCBMC.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTransMainForCBMC.cpp)
- [ILTransMainForTCGStateSearch.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTransMainForTCGStateSearch.cpp)

建议方式：

- 在现有 branch id 到 property label 的生成阶段补充记录
- 不改变生成代码，只导出映射

#### 接入点 4：branch read/write globals

文件：

- [ILTranslatorForTCGStateSearch.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTranslatorForTCGStateSearch.cpp)

原因：

- 当前已经做了 branch relation 分析
- 这里最容易得到 branch 读取哪些全局变量、写入哪些全局变量

建议方式：

- 在现有生成 `TCGStateSearchBasicBranchRelation.cpp` 的同时，顺手导出 JSON

#### 接入点 5：distance metadata

文件：

- [ILTranslatorForTCGStateSearch.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTranslatorForTCGStateSearch.cpp)

建议方式：

- 保持现有 `transCodeBranchDistanceRegister` 生成逻辑不变
- 在计算 branch distance / array distance 时额外记录元数据

### 1.7 输出时机

推荐在各 translator 的 `translate(...)` 尾部统一写盘：

- `ILTranslatorForCBMC`
- `ILTranslatorForTCGStateSearch`
- `ILTranslatorForCoverage`
- `ILTranslatorForLibFuzzer`

更稳妥的实现方式是：

- 所有记录先进入 `ILTransMetadataExporter` 的静态容器
- 具体某个后端翻译结束时，调用 `flush(outputDir)`

### 1.8 对现有功能的影响

理论上应为零或接近零：

- 不改变原有生成文件
- 不影响编译逻辑
- 不影响 `cbmc.exe` 调用
- 不影响 `TCGStateSearchBasic.cpp` 行为

唯一新增的是输出目录多出若干 JSON/CSV 文件。


## 第二部分：IL-based slicer（仅针对选定的未覆盖 obligation）

### 2.1 目标

在现有 IL 分析基础上，新增 **“按需、按 obligation 生成局部语义切片”** 的能力：  
**不是**对全部分支或整张 IL 做切片，而是 **在运行/搜索产生覆盖反馈之后，仅对当前仍 uncovered 且被选入分析队列的 obligation** 构造紧凑输入，供后续规则系统或大模型做语义分析、策略选择与失败归因。

与整体闭环的关系可概括为：

`覆盖反馈（未覆盖集合） → 选择子集 obligation → IL 切片 →（第三部分）LLM / 规则分析 → 再驱动 CBMC / TCGStateSearch 等执行`

注意：

- slicer 是附加分析功能
- 不参与代码生成
- 不改变现有翻译器行为
- **默认不对已覆盖 obligation 重复切片**（除非显式调试开关）

### 2.2 原则

- **按需触发**：切片由「未覆盖 obligation 列表 + 选择策略」驱动，而非翻译阶段对全图扫描。
- 优先复用现有 IL 数据结构
- 优先复用现有 `ILAnalyzeSchedule`、`ILTranslatorUtility`
- 不新增新的 XML 前端
- 不要求一开始做完整程序切片
- 先做 **obligation-centered、轻量、稀疏** 的切片；完整 backward slice 可作为远期增强

#### 「只针对某些未覆盖分支」的界定建议

- **覆盖事实来源**（实现时可二选一或组合）：运行期 `BitmapCov` 状态、CBMC/搜索日志中的未满足 property、或外部给出的 `bitmap_index` 列表。（Hybrid 下「停滞写请求文件」见 **2.9**。）
- **选择策略**（控制成本）：例如按优先级（距离小、历史接近成功、关键函数）、按批次上限、或仅对「上一轮策略推荐后仍失败」的 obligation 再切片。
- **与元数据的关系**：第一部分导出的 `obligation_map.json`、`branch_relation.json`、`distance_map.json` 提供 **静态** 语义与结构；第二部分在 **已知某 `bitmap_index` 仍 uncovered** 的前提下，从 IL 中抽取 **动态分析所需的局部上下文**，二者拼接后再交给 LLM。

### 2.3 建议新增文件

建议在 [D:\CodeGenerator2\Src\ILTranslator](D:\CodeGenerator2\Src\ILTranslator) 下新增：

- [ILObligationSlicer.h](/D:/CodeGenerator2/Src/ILTranslator/ILObligationSlicer.h)
- [ILObligationSlicer.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILObligationSlicer.cpp)

建议新增一个切片数据模型文件：

- [ILObligationSlice.h](/D:/CodeGenerator2/Src/ILTranslator/ILObligationSlice.h)

### 2.4 输入输出建议

#### 输入

- `ILParser*`（或等价：已加载的 IL 模型）
- **必选**：一个或多个 **当前未覆盖** 的 obligation 标识（如 `bitmap_index`，与第一部分 `obligations[].bitmap_index` 一致）
- **必选**：当前覆盖向量或「未覆盖 index 集合」（用于校验请求、避免对已覆盖项做无效切片）
- 可选：`obligation_map` / `branch_relation` / `distance_map` 中已导出的条目（减少重复计算）
- 可选：历史失败摘要（步数、后端、上次 horizon），供切片侧附加进 `slice_text` 而不强迫 LLM 再读大日志

#### 输出

输出两种形式：

- 结构化对象 `ILObligationSlice`（每个 **请求的** 未覆盖 obligation 一条，或合并为 batch）
- 文本化摘要 `slice.txt` 或 JSON 中的 `slice_text`

**说明**：一次调用可只处理 **用户或上层调度器选中的** 若干 `bitmap_index`，不要求输出全部分支的切片。

### 2.5 切片建议包含的信息

针对 **单个被选中的未覆盖 obligation**，一个 slice 至少应包含：

- obligation id（`bitmap_index`）及可选 `cbmc_property_label`
- uncovered 标记与可选「上次失败原因」占位（可由上层填入）
- function name
- file name
- condition text（与 metadata 中人类可读描述对齐或引用）
- control context（父级 if/switch/循环等）
- related inputs
- related persistent states（如 `*_DW` 等与该分支相关的状态）
- relevant statements（轻量：邻近若干条或 schedule 子图，而非整函数）
- read globals / write globals（与 `branch_relation` 可互证）
- whether likely multi-step（启发式字段即可，精判定交给第三部分 LLM）

### 2.6 推荐复用的现有能力

#### A. 调用深度 / 分支深度

文件：

- [ILTranslatorUtility.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTranslatorUtility.cpp)

可复用：

- `buildILFunctionCallGraph(...)`
- `getStatementCallDepth(...)`

#### B. schedule / data-flow 分析

当前 `TCGStateSearch` 已有分支相关的数据依赖分析。

文件：

- [ILTranslatorForTCGStateSearch.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTranslatorForTCGStateSearch.cpp)
- [ILTranslatorForTCGStateSearch.h](/D:/CodeGenerator2/Src/ILTranslator/ILTranslatorForTCGStateSearch.h)

可复用：

- `ILAnalyzeSchedule`
- branch read global variable collection
- branch write global variable collection

#### C. 条件表达式文本

来源建议：

- 直接从 branch / statement 的表达式翻译逻辑中提取
- 或重用表达式转字符串逻辑

### 2.7 最小实现方案

第一阶段不要做完整 backward slicer，只做 **「未覆盖 obligation → 轻量局部切片」**。

步骤建议：

1. **入口**：接收「未覆盖 `bitmap_index` 列表」+ `ILParser*`（及可选已加载的第一部分 JSON）。
2. **过滤**：丢弃已在覆盖向量中标记为已满足的 index；可对列表按 2.2 策略截断到每轮上限 `K` 条。
3. 对每个保留的 index：根据 IL 与 branch 映射找到对应 branch / statement。
4. 提取：
   - function
   - file
   - condition
   - parent control context
5. 从 metadata 或等价分析中补：
   - related globals
   - related inputs
   - branch depth / call depth
6. 输出结构化对象 + 一段可喂给 LLM 的 `slice_text`（可多 obligation 拼接为一份 batch 摘要，仍只覆盖**选中**的未覆盖项）。

示例（单个未覆盖 obligation）：

```text
Coverage status: UNCOVERED (bitmap_index=45)

Target obligation:
branch_45

Function:
FMTM_step

Condition:
(!Switch_l4) && (Switch_m != -1)

Control Context:
- switch(FMTM_U_Opcode)
- case 1
- else branch

Related Inputs:
- FMTM_U_Opcode
- FMTM_U_SenserId

Related Persistent States:
- FMTM_DW->SenserIdData
- FMTM_DW->SenserIdUsed

Observed Dependency:
- target branch reads persistent state written by earlier loop iterations
```

### 2.8 对现有功能的影响

应保持为零：

- slicer 不参与翻译
- slicer 不改变生成代码
- slicer 作为额外命令或额外输出存在

建议实现方式：

- 由 **单独工具入口** 或 **测试调度脚本** 在拿到覆盖反馈后调用：`未覆盖 index 列表 → slicer`
- 或由 `ILTranslator` 增加纯分析模式开关，例如 `-dump_slice`，且 **必须与 `-slice_indices` 或输入文件列出未覆盖 obligation 配合使用**，避免默认全量 dump

如果要更稳妥，建议先完全独立于主翻译流程实现。

### 2.9 当前已落地的桥接：TCGHybrid 内「覆盖停滞 → 切片请求文件」（非 IL 切片执行）

本节记录 **已实现** 与 **尚未实现** 的边界，避免与 2.3–2.7 中的 `ILObligationSlicer` 混淆。

#### 设计意图

- Hybrid 可执行文件内 **不含** `ILParser` / IL XML，无法在进程内直接执行「IL 语义切片」。
- 因此先在 **运行主循环** 中做 **触发器**：当聚合覆盖长时间无进展时，写出一份 **机器可读请求**，供 **外部进程**（独立 slicer 工具、`ILTranslator` 分析模式脚本等）结合第一部分的 `obligation_map.json` 再去做真正的切片与 LLM 输入组装。

#### 实现位置（模板源，非 `_generated_out` 手改）

- [TCGHybridBasic.cpp](/D:/CodeGenerator2/Src/exe/ILTranslator/ResourceCode/TCGHybridBasic.cpp)
- [TCGHybridBasic.h](/D:/CodeGenerator2/Src/exe/ILTranslator/ResourceCode/TCGHybridBasic.h)

由 [ILTranslatorForTCGHybrid.cpp](/D:/CodeGenerator2/Src/ILTranslator/ILTranslatorForTCGHybrid.cpp) 的 `copyResourceFiles` 将上述文件拷至翻译输出目录；重新翻译 Hybrid 工程后生效。

#### 触发条件（与代码一致）

- **基准**：在 `initTCGHybrid2()` 末尾记录当前 `BitmapCov` 的聚合计数快照：`munitCov`、`branchCov`、`condCov`、`decCov`、`mcdcCov`。
- **每步**：在 `stepTCGHybrid()` 末尾（`minimizeTestCasesBinary`、`randomRun`、STCG、休眠、ParaFuzzer 等 **本步全部执行完毕之后**）调用 `hybridCoverageStallSliceTickAfterStep()`。
- **「覆盖无变化」**：上述五个计数 **较上次快照均无严格增长**（任一项变大则视为有进展，刷新快照并重置计时）。
- **时间阈值**：自上次有进展起，`clock()` 差值 **≥ 10 × `CLOCKS_PER_SEC`** 则触发写文件（随后重置计时；若仍停滞，可再次每隔约 10s 写出，便于外部轮询）。

**说明**：此处用的是 **`clock()` 的进程时间语义**；若需严格「墙上挂钟 10 秒」，可后续改为 `std::chrono::steady_clock`（文档不绑死实现）。

#### 产出文件（供外部切片管线消费）

- 目录：`{exe 工作目录}/SliceOut/`
- 文件：`hybrid_coverage_stall_slice_request.json`
- 主要内容建议包含（与实现一致）：
  - `trigger`：如 `hybrid_coverage_stall_10s`
  - `aggregate_coverage`：当前五个聚合计数
  - `uncovered_branch_bitmap_indices`：当前 `branchAllBitmap` 上仍为 0 的槽位下标，与第一部分 **`obligations[].bitmap_index`** 对齐
  - `note`：提示与 `obligation_map.json` 配对使用，IL 切片在 **本二进制外** 完成

同时可向 `log.txt` 追加 `[HybridSlice]` 日志行，便于人工排查。

#### 仍未实现（仍属 2.3–2.7 规划）

- `ILObligationSlicer` / `ILObligationSlice`：**未编写**；未从 IL 抽取控制上下文、`slice_text` 等。
- Hybrid 内 **不会** 调用 LLM；第三部分仍由独立分析器消费「metadata + 外部切片产物 + 本 JSON 请求」。

#### 与闭环的衔接关系

`TCGHybrid 主循环 →（停滞）hybrid_coverage_stall_slice_request.json → 外部工具按 bitmap_index 查 obligation_map + 加载 IL → 真切片 / LLM`  

其中从「请求文件」到「真切片」的一步 **有意留空**，由后续实现 `ILObligationSlicer` 或脚本补齐。


## 第三部分：大模型分析器

### 3.1 目标

在不替代现有 solver / search / fuzz 后端的前提下，引入一个大模型分析层，负责 obligation 级别的：

- 分类
- 策略推荐
- 失败归因

而不是直接生成底层测试值。  
输入中的 **IL 切片**应对应当前轮 **选定的未覆盖 obligation**（见第二部分），而非整图全文。

### 3.2 原则

- 大模型不改写现有运行流程
- 大模型不直接代替 `cbmc.exe`
- 大模型不直接代替 `TCGStateSearch`
- 大模型只消费：
  - metadata
  - slice（由第二部分按未覆盖子集生成）
  - 历史失败摘要

### 3.3 建议新增位置

这一层不建议先写进 `ILTranslator` C++ 主体里。

更合适的方式是单独做一个旁路分析器。

建议新增目录：

- [D:\CodeGenerator2\Tools\LLMAnalyzer](D:\CodeGenerator2\Tools\LLMAnalyzer)

建议文件：

- [analyzer.py](/D:/CodeGenerator2/Tools/LLMAnalyzer/analyzer.py)
- [schemas.py](/D:/CodeGenerator2/Tools/LLMAnalyzer/schemas.py)
- [prompt_templates.md](/D:/CodeGenerator2/Tools/LLMAnalyzer/prompt_templates.md)

如果暂时不想引入 Python，也可以先只写成设计接口，不落代码。

### 3.4 输入

大模型输入建议只包括：

- `obligation_map.json` 中的目标 obligation 条目
- slicer 输出文本
- 最近若干次执行摘要
- 可选的 distance progress

不要直接输入：

- 整份 XML
- 整份 `FMTM.cpp`
- 整个输出工程目录

### 3.5 输出任务设计

#### A. Branch Classifier

输出：

- `single_step_input_driven`
- `multi_step_state_dependent`
- `mode_entry_dependent`
- `state_accumulation_required`
- `suspected_infeasible`

示例输出：

```json
{
  "branch_type": "multi_step_state_dependent",
  "relevant_inputs": ["FMTM_U_Opcode", "FMTM_U_SenserId"],
  "relevant_states": ["FMTM_DW->SenserIdData", "FMTM_DW->SenserIdUsed"],
  "recommended_engine": "tcg_state_search",
  "recommended_horizon": 6,
  "reason": "target branch depends on persistent array state populated across earlier steps"
}
```

#### B. Strategy Recommender

输出：

- `cbmc_single_step`
- `cbmc_multi_step`
- `tcg_state_search`
- `libfuzzer`
- `hybrid`

以及：

- 推荐 horizon
- 是否优先构造前缀状态
- 是否优先关注某些状态变量

#### C. Failure Explainer

基于运行历史输出：

- 失败类型
- 失败原因
- 下一轮建议

示例：

```json
{
  "failure_type": "state_not_ready",
  "reason": "persistent state required by the target branch was not populated before the query step",
  "suggestion": "increase prefix horizon and prioritize state initialization before activating the target condition"
}
```

### 3.6 与现有系统的集成方式

建议采用最小侵入方式：

1. 先跑现有 `ILTranslator`
2. 生成 metadata
3. 用 slicer 生成目标 obligation 的局部切片
4. 外部分析器读取 metadata + slice
5. 输出 JSON 分析结果
6. 调度脚本再根据 JSON 结果选择：
   - `main.c`
   - `main_2.c`
   - `outSTCG`
   - `outFuzz`

这样不会改动现有 `ILTranslator` 主路径。

### 3.7 对现有功能的影响

这一层应做到完全旁路：

- 不改 C++ 生成逻辑
- 不改 `TCGStateSearchBasic.cpp`
- 不改 `CBMC` 调用逻辑
- 不改 `FMTM` 现有输出工程

它只是对现有产物做额外分析，并给调度层提供建议。


## 实施优先级建议

在“尽量不改原有功能”的前提下，推荐顺序如下：

1. 元数据导出
2. 轻量 IL-based slicer
3. 大模型分析器

原因是：

- 元数据是地基
- slicer 是给大模型减负的必要中间层
- 大模型分析器应尽量最后接入


## 最终建议

如果你特别强调“不要动原有功能”，那最稳妥的落地方式是：

- `ILTranslator` 只新增导出，不改生成语义
- slicer 独立实现，不进入主翻译流程
- 大模型分析器完全作为外部工具存在

这样三部分都能做出来，同时对现有 `FMTM -> CBMC/TCGStateSearch` 工作流影响最小。
