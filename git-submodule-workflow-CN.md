# Git 子模块工作流说明

这份文档专门说明这个仓库里为什么 `python/training/classification`、`ros`、`utils` 这些目录不能像普通文件夹那样直接跟着主仓库一起 push，以及怎样把它们也放到 `mylearn` 分支上并简化提交流程。

## 先说结论

这些目录不是普通目录，而是 **Git 子模块**。

在这个仓库里，比较明显的子模块有：

- [utils](g:/jetson-inference/utils)
- [ros](g:/jetson-inference/ros)
- [python/training/classification](g:/jetson-inference/python/training/classification)
- [python/training/detection](g:/jetson-inference/python/training/detection)
- [python/training/segmentation](g:/jetson-inference/python/training/segmentation)
- [tools/camera-capture](g:/jetson-inference/tools/camera-capture)

它们在 [.gitmodules](g:/jetson-inference/.gitmodules) 里有单独定义。

所以要记住一件事：

- 主仓库是一份 Git 历史
- 每个子模块又是另一份独立 Git 历史

这意味着：

- 你不能只在主仓库 `git push` 一次，就把所有子模块内容一起推上去
- 子模块里的改动，要先在子模块仓库里提交
- 然后主仓库再提交新的“子模块指针”

## 为什么你会觉得“改了文件却 push 不上去”

因为主仓库看到的不是“子模块里面某个文件改了”，而是：

- 这个子模块当前工作区有改动
- 或者这个子模块已经移动到了一个新的 commit

所以在主仓库里你常会看到这种状态：

- `modified: ros (untracked content)`
- `modified: python/training/classification (untracked content)`

这不是说主仓库直接包含了这些文件内容，而是说：

- 子模块内部变了
- 但主仓库还没记录这个变化

## “把子模块也弄到 mylearn 分支上”到底是什么意思

这里其实有两层意思：

### 1. 子模块里也创建或切换到 `mylearn`

例如：

- 主仓库在 `mylearn`
- `ros` 子模块也切到 `mylearn`
- `python/training/classification` 子模块也切到 `mylearn`

这样你开发时心智会统一一些。

### 2. 子模块的远端也要可 push

只有切到 `mylearn` 还不够。

如果子模块的 `origin` 还是上游仓库，而你对上游没写权限，那你在子模块里依然 push 不上去。

所以通常还需要：

- 给每个子模块 fork 一个你自己的仓库
- 把子模块的 `origin` 改成你的 fork

## 推荐工作流

推荐按这个顺序做：

1. 先确认每个子模块的 `origin` 是不是你自己的 fork
2. 在子模块里创建或切换到 `mylearn`
3. 在子模块里提交并 push
4. 回主仓库提交子模块指针
5. push 主仓库

## 第一步：确认子模块远端是不是你的 fork

例如检查 `ros`：

```bash
git -C ros remote -v
```

检查 `classification`：

```bash
git -C python/training/classification remote -v
```

如果输出还是上游作者的仓库，而不是你的 GitHub 仓库，那你先要改成自己的 fork。

例如：

```bash
git -C ros remote set-url origin https://github.com/<你的用户名>/ros_deep_learning.git
git -C python/training/classification remote set-url origin https://github.com/<你的用户名>/pytorch-classification.git
```

如果不改这一步，后面即使 commit 成功，push 也会被权限拒绝。

## 第二步：在子模块里创建或切换到 `mylearn`

最原始的做法就是分别执行：

```bash
git -C ros checkout -b mylearn
git -C python/training/classification checkout -b mylearn
```

如果分支已经存在，就改成：

```bash
git -C ros checkout mylearn
git -C python/training/classification checkout mylearn
```

## 第三步：在子模块里提交

例如：

```bash
git -C ros add README-CN.md
git -C ros commit -m "add Chinese README"
git -C ros push -u origin mylearn

git -C python/training/classification add README-CN.md
git -C python/training/classification commit -m "add Chinese README"
git -C python/training/classification push -u origin mylearn
```

## 第四步：回主仓库记录子模块指针

子模块 push 完以后，主仓库要记录“现在它们应该指向哪个新 commit”：

```bash
git add ros python/training/classification
git commit -m "update submodule pointers"
git push origin mylearn
```

这一步不能省。

## 能不能“一键提交”

严格来说，**不能做到真正意义上的单个 Git 提交**。

因为 Git 子模块本质上就是多个独立仓库，所以至少会有：

- 子模块自己的 commit
- 主仓库记录子模块指针的 commit

但你可以做到：

- 一条脚本命令
- 自动按顺序把这些仓库都提交并 push

这就是“串行一键提交”，不是“所有仓库共享一个 commit”。

## 这个仓库里我给你加了一个辅助脚本

脚本位置：

- [scripts/git-submodule-helper.ps1](g:/jetson-inference/scripts/git-submodule-helper.ps1)

它主要做两件事：

- 把指定子模块切到某个分支
- 自动提交子模块，再提交主仓库

## 脚本怎么用

### 1. 查看当前子模块状态

```powershell
.\scripts\git-submodule-helper.ps1 -Action status
```

这会列出每个子模块当前所在分支，以及是不是 dirty。

### 2. 把指定子模块切到 `mylearn`

```powershell
.\scripts\git-submodule-helper.ps1 -Action branch -Branch mylearn -Submodules ros,python/training/classification
```

如果：

- 本地已有 `mylearn`
  它会直接切过去

- 本地没有但远端有
  它会尝试跟踪远端

- 本地和远端都没有
  它会从当前 HEAD 新建

### 3. 一键提交并 push 子模块，再提交主仓库

```powershell
.\scripts\git-submodule-helper.ps1 -Action commit-push -Branch mylearn -Submodules ros,python/training/classification -Message "docs: update Chinese docs"
```

它会按这个顺序做：

1. 进入每个子模块
2. 确保子模块在 `mylearn`
3. `git add -A`
4. 如果有变更就 commit
5. push 到该子模块的 `origin/mylearn`
6. 回到主仓库
7. `git add` 这些子模块路径
8. 提交主仓库里的子模块指针变化
9. push 主仓库 `origin/mylearn`

### 4. 如果你还想顺手把主仓库自己的普通文件改动也一起提交

```powershell
.\scripts\git-submodule-helper.ps1 -Action commit-push -Branch mylearn -Submodules ros,python/training/classification -Message "docs: update Chinese docs" -IncludeRootChanges
```

默认情况下，脚本只会把你指定的子模块路径加到主仓库提交里。  
加上 `-IncludeRootChanges` 后，它会对主仓库执行 `git add -A`。

### 5. 如果你只想 commit，不想 push

```powershell
.\scripts\git-submodule-helper.ps1 -Action commit-push -Branch mylearn -Submodules ros,python/training/classification -Message "docs: update Chinese docs" -NoPush
```

## 这个脚本的边界

它只是帮你把流程串起来，不会替你解决这些问题：

### 1. 远端没有写权限

如果子模块 `origin` 不是你的 fork，push 还是会失败。

### 2. 仓库安全目录问题

如果 Git 报：

- `detected dubious ownership`

说明当前 Windows 用户和仓库拥有者不一致，需要你自己把仓库加入 safe.directory。

例如：

```bash
git config --global --add safe.directory G:/jetson-inference/ros
git config --global --add safe.directory G:/jetson-inference/python/training/classification
git config --global --add safe.directory G:/jetson-inference/utils
```

### 3. 不是所有子模块都应该跟主仓库同名分支

技术上可以都叫 `mylearn`，但是否合理取决于你的维护方式。

如果某个子模块只是临时改一下文档，你也可以用：

- `docs-cn`
- `mylearn-docs`

不一定非要全都叫 `mylearn`。

## 对你当前情况的建议

你现在最适合这样做：

1. 先给你实际会改的子模块 fork 自己的仓库
   - `ros`
   - `python/training/classification`
   - 如果后面要改 `utils`，也 fork `utils`

2. 把这些子模块的 `origin` 改成你的 fork

3. 用脚本先切分支

```powershell
.\scripts\git-submodule-helper.ps1 -Action branch -Branch mylearn -Submodules ros,python/training/classification
```

4. 再用脚本做串行提交

```powershell
.\scripts\git-submodule-helper.ps1 -Action commit-push -Branch mylearn -Submodules ros,python/training/classification -Message "docs: add Chinese README files"
```

## 最后一句话总结

子模块不能像普通目录那样跟主仓库“天然一键 push”，因为它们本来就是独立仓库。

你能做的是：

- 让它们也切到 `mylearn`
- 把远端改成你自己的 fork
- 用脚本把“子模块 commit/push + 主仓库 commit/push”串起来

这样实际使用上就已经接近你想要的“一键提交”了。
