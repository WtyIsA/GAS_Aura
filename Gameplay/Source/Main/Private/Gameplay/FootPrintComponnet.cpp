#include "FootPrintComponent.h"
#include "LuaManger.h"

UFootPrintComponent::UFootPrintComponent()
{
}

void UFootPrintComponent::AddFootPrint(FVector Location,const double TimeSeconds,const uint8 Status)
{
	/*const FHistoryStatus HistoryStatus(Location,TimeSeconds,Status);
	HistoryStatuses.EmplaceLast(HistoryStatus);
	while(true)
	{
		const FHistoryStatus& OldStatus = HistoryStatuses.First();
		if(TimeSeconds - OldStatus.TimeSeconds >= MaxDeltaTime)
		{
			HistoryStatuses.PopFirst();
		}
		else
		{
			break;
		}
	}*/
}

bool UFootPrintComponent::IsInRange(const FVector Location,const int32 Status,const float Radius,const float Height) const
{
	const double TimeSeconds = FPlatformTime::Seconds();
	/*if(const int32 Num = HistoryStatuses.Num())
	{
		float MinX = 0;
		float MinY = 0;
		float MinZ = 0;
		float MaxX = 0;
		float MaxY = 0;
		float MaxZ = 0;
		bool HasValue = false;
		bool HasStatus = false;
		for(int32 Index=Num-1;Index>=0;--Index)
		{
			const FHistoryStatus History = HistoryStatuses[Index];
			if(HasValue == false)
			{
				MinX = MaxX = History.Location.X;
				MinY = MaxY = History.Location.Y;
				MinZ = MaxZ = History.Location.Z;
				if(History.Status == Status)
				{
					HasStatus = true;
				}
			}		
			if(TimeSeconds - History.TimeSeconds >= MaxDeltaTime)
			{
				break;
			}else
			{
				if(!HasValue)
				{
					HasValue = true;
				}
				else
				{
					if(History.Status == Status)
					{
						HasStatus = true;
					}					
					MinX = FMath::Min(MinX,History.Location.X);
					MinY = FMath::Min(MinY,History.Location.Y);
					MinZ = FMath::Min(MinZ,History.Location.Z);
					
					MaxX = FMath::Max(MaxX,History.Location.X);
					MaxY = FMath::Max(MaxY,History.Location.Y);
					MaxZ = FMath::Max(MaxZ,History.Location.Z);
				}
			}
		}
		if(!CheckStatus || HasStatus)
		{			
			MinX -= Radius;
			MaxX += Radius;	
			MinY -= Radius;
			MaxY += Radius;
			MinZ -= Height;
			MaxZ += Height;
			return FMath::IsWithinInclusive(Location.X,MinX,MaxX) && 
					FMath::IsWithinInclusive(Location.Y,MinY,MaxY) && 
					FMath::IsWithinInclusive(Location.Z,MinZ,MaxZ);			
		}
	}*/
	return false;
}

int32 UFootPrintComponent::Size() const
{
	return -1;
	//return HistoryStatuses.Num();
}
