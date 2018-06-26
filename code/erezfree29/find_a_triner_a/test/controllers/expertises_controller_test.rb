require 'test_helper'

class ExpertisesControllerTest < ActionDispatch::IntegrationTest
  test "should get index" do
    get expertises_index_url
    assert_response :success
  end

  test "should get show" do
    get expertises_show_url
    assert_response :success
  end

  test "should get new" do
    get expertises_new_url
    assert_response :success
  end

  test "should get create" do
    get expertises_create_url
    assert_response :success
  end

  test "should get edit" do
    get expertises_edit_url
    assert_response :success
  end

  test "should get update" do
    get expertises_update_url
    assert_response :success
  end

  test "should get destroy" do
    get expertises_destroy_url
    assert_response :success
  end

end
