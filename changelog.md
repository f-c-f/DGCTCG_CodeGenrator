# Changelog

本文档总结对 CodeGenerator2 项目（尤其是 ILTranslator 及相关解析/生成逻辑）的修改。

---

## 1. 修复 vector 越界与断言失败

### 1.1 ILParser.cpp

- **parseStmtExpression**  
  - 在访问 `statement->innerExpressions[0]` 前增加 `innerExpressions.empty()` 判断。  
  - 若为空则返回 `-ErrorStatementInvalidType`，避免在缺少 `Expression` 属性或解析未产生表达式时触发 “vector subscript out of range”。

- **parseStmtFor**  
  - 在访问 `childStatements[0]`、`[1]`、`[2]` 前增加 `childStatements.size() < 3` 检查。  
  - 不满足时返回 `-ErrorStatementForHasNoStatementIterator`，避免 For 结构不完整时越界。

### 1.2 Expression.cpp

- **updateExpressionStr()**  
  - 在按 `type` 使用 `childExpressions[0]` / `[1]` 的所有分支前增加长度检查（`childExpressions.size() < 1` 或 `< 2` 时直接 `return`）。  
  - 避免在子节点数量与类型不匹配时触发 “vector subscript out of range” 断言。

### 1.3 ILTransMain.cpp

- **translateMainInput（Simulator 路径）**  
  - 不再使用 `basicDataTypeWidthMap.find(type)->second`（在 type 不在 map 时可能解引用 end()）。  
  - 改为使用 `ILParser::getBasicDataTypeWidth(type)`，若返回值 ≤ 0 则用默认值 8。

---

## 2. memset 未声明与头文件

生成代码中使用了 `memset`（如 XML 中的 `(void)memset(...)`），需保证生成文件包含 `<cstring>`（C++）或 `<string.h>`（C），且不依赖 define.h 的 include 顺序或 guard。

### 2.1 在生成内容中增加 `<cstring>`

- **ILTranslatorForCBMC.cpp**  
  - 若 `transCodeHeadFile` 中尚未包含 `<cstring>`，则在其前部添加 `#include <cstring>`。

- **ILTransMainForCoverage.cpp**  
  - 在原有 `#include <string.h>` 基础上，再追加 `#include <cstring>`。

- **ILTransMainForTCGStateSearch.cpp**  
  - 同上，保留 `<string.h>` 并追加 `#include <cstring>`。

- **ILTransMainForTCGHybrid.cpp**  
  - 同上，保留 `<string.h>` 并追加 `#include <cstring>`。

- **ILTranslatorForC.cpp**  
  - 若 `defineCode` 中尚未包含 `<cstring>`，则在前面添加 `#include <cstring>`。

- **ILTranslatorForSimulator.cpp**  
  - 若 `defineCode` 中尚未包含 `<cstring>`，则在前面添加 `#include <cstring>`。

### 2.2 在每个生成的 .cpp 文件最前添加 `<cstring>`

为避免 `define.h` 被 include guard 跳过导致 `<cstring>` 未生效，在**每个生成的 .cpp 源文件内容的最前面**直接插入 `#include <cstring>`：

- **ILTranslatorForCoverage.cpp**  
  - 每个 .cpp（如 `AM_Mod_Demod.cpp`）：在 `#include "BitmapCov.h"` 之前插入 `#include <cstring>`。  
  - `main.cpp`：在 `#include "define.h"` 之前插入 `#include <cstring>`。

- **ILTranslatorForTCGStateSearch.cpp**  
  - 每个 .cpp：在 `#include "TCGStateSearchBasic.h"` 之前插入 `#include <cstring>`。  
  - `main.cpp`：在 `#include "define.h"` 之前插入 `#include <cstring>`。

- **ILTranslatorForTCGHybrid.cpp**  
  - 每个 .cpp：在 `#include "BitmapCov.h"` 之前插入 `#include <cstring>`。  
  - `main.cpp`：在 `#include "define.h"` 之前插入 `#include <cstring>`。

---

## 3. 已回退的修改（按用户要求）

以下修改曾为缓解 “重复声明” 或 “变量冲突” 而做，后按用户要求撤销：

- **ILParser.cpp — parseStmtLocalVariable**  
  - 曾放宽：当 `addVariableName` 返回 `-ErrorVariableConflict` 时不报错、继续解析（允许多个同名局部变量如多个 for 的 `i`）。  
  - **已恢复**：仍按原逻辑，一旦 `addVariableName` 返回 &lt; 0 即返回错误。

- **ILTransStatement.cpp — translateStmtLocalVariable**  
  - 曾增加：若同一父节点下前面已有同名 `LocalVariable`，则不再生成该变量的声明，避免 C 中重复定义。  
  - **已恢复**：删除该逻辑，每个 LocalVariable 仍正常生成声明。

用户已在 XML 侧解决重复声明问题，故上述放宽/特殊处理已取消。

---

## 涉及文件列表

- `Src/ILBasic/ILParser.cpp`
- `Src/ILBasic/Expression.cpp`
- `Src/ILTranslator/ILTransMain.cpp`
- `Src/ILTranslator/ILTransStatement.cpp`
- `Src/ILTranslator/ILTranslatorForCBMC.cpp`
- `Src/ILTranslator/ILTranslatorForC.cpp`
- `Src/ILTranslator/ILTranslatorForCoverage.cpp`
- `Src/ILTranslator/ILTranslatorForSimulator.cpp`
- `Src/ILTranslator/ILTranslatorForTCGStateSearch.cpp`
- `Src/ILTranslator/ILTranslatorForTCGHybrid.cpp`
- `Src/ILTranslator/ILTransMainForCoverage.cpp`
- `Src/ILTranslator/ILTransMainForTCGStateSearch.cpp`
- `Src/ILTranslator/ILTransMainForTCGHybrid.cpp`
