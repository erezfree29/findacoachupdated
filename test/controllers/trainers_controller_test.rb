require 'test_helper'

class TrainersControllerTest < ActionDispatch::IntegrationTest
#   test "should get index" do
#     get trainers_index_url
#     assert_response :success
#   end

#   test "should get show" do
#     get trainer_url
#     assert_response :success
#   end

#   test "should get new" do
#     get new_trainer_url
#     assert_response :success
#   end

#   test "should get create" do
#     get new_trainer_url
#     assert_response :success
#   end

#   test "should get edit" do
#     get edit_trainer_url
#     assert_response :success
#   end

#   test "should get update" do
#     get trainers_update_url
#     assert_response :success
#   end

#   test "should get destroy" do
#     get trainers_destroy_url
#     assert_response :success
#   end

# test "redirect to trainer" do
#     get trainers_destroy_url
#     assert_response :success
#   end

 test "can not save without a name" do
     trainer = Trainer.new()
   
     assert_not trainer.save
     

   end

   test "gives a flash notice" do
     trainer = Trainer.new()
   
    assert_equal "Your fee is valid till", flash[:info] 
     

   end



end



