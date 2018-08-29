require 'test_helper'

class TrainerTest < ActiveSupport::TestCase
  test "can not save without a name" do
     trainer = Trainer.new()
   
     assert_not trainer.save
     

   end


end
