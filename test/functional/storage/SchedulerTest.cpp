#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/storage/PartialScheduler.h"
#include "storm/storage/TotalScheduler.h"

TEST(SchedulerTest, PartialScheduler) {
    storm::storage::PartialScheduler scheduler;
    
    ASSERT_NO_THROW(scheduler.setChoice(0, 1));
    ASSERT_NO_THROW(scheduler.setChoice(0, 3));
    ASSERT_NO_THROW(scheduler.setChoice(3, 4));
    
    ASSERT_TRUE(scheduler.isChoiceDefined(0));
    ASSERT_EQ(3ul, scheduler.getChoice(0));
    
    ASSERT_TRUE(scheduler.isChoiceDefined(3));
    ASSERT_EQ(4ul, scheduler.getChoice(3));
    
    ASSERT_FALSE(scheduler.isChoiceDefined(1));
    ASSERT_THROW(scheduler.getChoice(1), storm::exceptions::InvalidArgumentException);
}

TEST(SchedulerTest, TotalScheduler) {
    storm::storage::TotalScheduler scheduler(4);
    
    ASSERT_NO_THROW(scheduler.setChoice(0, 1));
    ASSERT_NO_THROW(scheduler.setChoice(0, 3));
    ASSERT_NO_THROW(scheduler.setChoice(3, 4));
    
    ASSERT_TRUE(scheduler.isChoiceDefined(0));
    ASSERT_EQ(3ul, scheduler.getChoice(0));
    
    ASSERT_TRUE(scheduler.isChoiceDefined(3));
    ASSERT_EQ(4ul, scheduler.getChoice(3));
    
    ASSERT_TRUE(scheduler.isChoiceDefined(1));
    ASSERT_EQ(0ul, scheduler.getChoice(1));
                                             
    ASSERT_THROW(scheduler.getChoice(4), storm::exceptions::InvalidArgumentException);
    ASSERT_THROW(scheduler.setChoice(5, 2), storm::exceptions::InvalidArgumentException);
}
