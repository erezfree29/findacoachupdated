class ChangeTableName < ActiveRecord::Migration[5.2]
  def change
    rename_table :training_services, :trainers
  end
end
