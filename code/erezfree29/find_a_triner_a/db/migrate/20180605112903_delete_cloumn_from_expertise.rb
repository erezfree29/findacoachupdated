class DeleteCloumnFromExpertise < ActiveRecord::Migration[5.2]
  def change
    remove_column :expertises, :difficulty_level
  end
end
