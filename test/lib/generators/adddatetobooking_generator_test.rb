require 'test_helper'
require 'generators/adddatetobooking/adddatetobooking_generator'

class AdddatetobookingGeneratorTest < Rails::Generators::TestCase
  tests AdddatetobookingGenerator
  destination Rails.root.join('tmp/generators')
  setup :prepare_destination

  # test "generator runs without errors" do
  #   assert_nothing_raised do
  #     run_generator ["arguments"]
  #   end
  # end
end
