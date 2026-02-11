// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyController = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyController)
	{
		SendMouseCursorData();
	} else
	{
		// 为服务器，监听TargetData
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey PredictionKey = GetActivationPredictionKey();
		// 为指定的Ability和预测键绑定委托，当客户端数据到达时调用回调
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey)
			.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);
		// 检查是否已经有缓存的TargetData（数据在绑定前到达），若有立即触发回调
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
		if (!bCalledDelegate) // 如果没有缓存数据（数据在绑定后到达），设置等待远程玩家数据的状态
		{
			SetWaitingOnRemotePlayerData(); // 保持AbilityTask存活，等待数据到达后触发回调
		}
		// 客户端                            服务器
		// |                                |
		// | Activate()                     |
		// | 发现是本地控制                   |
		// | 调用SendMouseCursorData()       |
		// |                                |
		// | ServerSetReplicatedTargetData()|
		// | -----------RPC---------------->|
		// |                                | 接收RPC
		// |                                | 调用SetReplicatedTargetData()
		// |                                | 查找已绑定的委托
		// |                                | 触发AbilityTargetDataSetDelegate
		// |                                | 调用OnTargetDataReplicatedCallBack()
		// |                                | 广播ValidData委托
		// |                                |
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 在作用域内自动管理预测相关的状态和资源, 对象析构时会自动清理预测窗口相关的数据和状态
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult HitResult;
	PC->GetHitResultUnderCursor(ECC_Target, true, HitResult);

	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;

	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	// 将目标数据复制到服务器
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	// 消费（清除）客户端复制的TargetData，防止重复处理
	AbilitySystemComponent.Get()->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
